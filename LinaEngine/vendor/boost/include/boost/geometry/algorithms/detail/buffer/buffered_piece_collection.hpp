// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2012-2014 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2017 Adam Wulkiewicz, Lodz, Poland.

// This file was modified by Oracle on 2016-2019.
// Modifications copyright (c) 2016-2019 Oracle and/or its affiliates.
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_DETAIL_BUFFER_BUFFERED_PIECE_COLLECTION_HPP
#define BOOST_GEOMETRY_ALGORITHMS_DETAIL_BUFFER_BUFFERED_PIECE_COLLECTION_HPP

#include <algorithm>
#include <cstddef>
#include <set>

#include <boost/core/ignore_unused.hpp>
#include <boost/range.hpp>

#include <boost/geometry/core/assert.hpp>
#include <boost/geometry/core/coordinate_type.hpp>
#include <boost/geometry/core/point_type.hpp>

#include <boost/geometry/algorithms/comparable_distance.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/is_convex.hpp>

#include <boost/geometry/strategies/buffer.hpp>

#include <boost/geometry/geometries/ring.hpp>

#include <boost/geometry/algorithms/detail/buffer/buffered_ring.hpp>
#include <boost/geometry/algorithms/detail/buffer/buffer_policies.hpp>
#include <boost/geometry/algorithms/detail/overlay/cluster_info.hpp>
#include <boost/geometry/algorithms/detail/buffer/buffer_box.hpp>
#include <boost/geometry/algorithms/detail/buffer/get_piece_turns.hpp>
#include <boost/geometry/algorithms/detail/buffer/turn_in_piece_visitor.hpp>
#include <boost/geometry/algorithms/detail/buffer/turn_in_original_visitor.hpp>

#include <boost/geometry/algorithms/detail/disjoint/point_box.hpp>
#include <boost/geometry/algorithms/detail/overlay/add_rings.hpp>
#include <boost/geometry/algorithms/detail/overlay/assign_parents.hpp>
#include <boost/geometry/algorithms/detail/overlay/enrichment_info.hpp>
#include <boost/geometry/algorithms/detail/overlay/enrich_intersection_points.hpp>
#include <boost/geometry/algorithms/detail/overlay/ring_properties.hpp>
#include <boost/geometry/algorithms/detail/overlay/select_rings.hpp>
#include <boost/geometry/algorithms/detail/overlay/traversal_info.hpp>
#include <boost/geometry/algorithms/detail/overlay/traverse.hpp>
#include <boost/geometry/algorithms/detail/overlay/turn_info.hpp>
#include <boost/geometry/algorithms/detail/partition.hpp>
#include <boost/geometry/algorithms/detail/sections/sectionalize.hpp>
#include <boost/geometry/algorithms/detail/sections/section_box_policies.hpp>

#include <boost/geometry/views/detail/normalized_view.hpp>
#include <boost/geometry/util/range.hpp>

namespace boost { namespace geometry
{


#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace buffer
{


/*
 *  Terminology
 *
 *  Suppose we make a buffer (using blocked corners) of this rectangle:
 *
 *         +-------+
 *         |       |
 *         |  rect |
 *         |       |
 *         +-------+
 *
 * For the sides we get these four buffered side-pieces (marked with s)
 * and four buffered corner pieces (marked with c)
 *
 *     c---+---s---+---c
 *     |   | piece |   |     <- see below for details of the middle top-side-piece
 *     +---+-------+---+
 *     |   |       |   |
 *     s   |  rect |   s     <- two side pieces left/right of rect
 *     |   |       |   |
 *     +---+-------+---+
 *     |   | piece |   |     <- one side-piece below, and two corner pieces
 *     c---+---s---+---c
 *
 *  The outer part of the picture above, using all pieces,
 *    form together the offsetted ring (marked with o below)
 *  The 8 pieces are part of the piece collection and use for inside-checks
 *  The inner parts form (using 1 or 2 points per piece, often co-located)
 *    form together the robust_polygons (marked with r below)
 *  The remaining piece-segments are helper-segments (marked with h)
 *
 *     ooooooooooooooooo
 *     o   h       h   o
 *     ohhhrrrrrrrrrhhho
 *     o   r       r   o
 *     o   r       r   o
 *     o   r       r   o
 *     ohhhrrrrrrrrrhhho
 *     o   h       h   o
 *     ooooooooooooooooo
 *
 */


template
<
    typename Ring,
    typename IntersectionStrategy,
    typename DistanceStrategy,
    typename RobustPolicy
>
struct buffered_piece_collection
{
    typedef typename geometry::point_type<Ring>::type point_type;
    typedef typename geometry::coordinate_type<Ring>::type coordinate_type;
    typedef typename geometry::robust_point_type
    <
        point_type,
        RobustPolicy
    >::type robust_point_type;

    // Robust ring/polygon type, always clockwise
    typedef geometry::model::ring<robust_point_type> robust_ring_type;
    typedef geometry::model::box<robust_point_type> robust_box_type;

    typedef typename default_comparable_distance_result
        <
            robust_point_type
        >::type robust_comparable_radius_type;

    typedef typename IntersectionStrategy::side_strategy_type side_strategy_type;
    typedef typename IntersectionStrategy::envelope_strategy_type envelope_strategy_type;
    typedef typename IntersectionStrategy::expand_strategy_type expand_strategy_type;

    typedef typename IntersectionStrategy::template area_strategy
        <
            point_type
        >::type area_strategy_type;

    typedef typename IntersectionStrategy::template area_strategy
        <
            robust_point_type
        >::type robust_area_strategy_type;

    typedef typename area_strategy_type::template result_type
        <
            point_type
        >::type area_result_type;
    typedef typename robust_area_strategy_type::template result_type
        <
            robust_point_type
        >::type robust_area_result_type;

    typedef typename IntersectionStrategy::template point_in_geometry_strategy
        <
            robust_point_type,
            robust_ring_type
        >::type point_in_geometry_strategy_type;

    typedef typename geometry::rescale_policy_type
        <
            typename geometry::point_type<Ring>::type,
            typename IntersectionStrategy::cs_tag
        >::type rescale_policy_type;

    typedef geometry::segment_ratio
    <
        typename geometry::coordinate_type<robust_point_type>::type
    > ratio_type;

    typedef buffer_turn_info
    <
        point_type,
        robust_point_type,
        ratio_type
    > buffer_turn_info_type;

    typedef buffer_turn_operation
    <
        point_type,
        ratio_type
    > buffer_turn_operation_type;

    typedef std::vector<buffer_turn_info_type> turn_vector_type;

    struct robust_turn
    {
        std::size_t turn_index;
        int operation_index;
        robust_point_type point;
        segment_identifier seg_id;
        ratio_type fraction;
    };

    struct piece
    {
        typedef robust_ring_type piece_robust_ring_type;
        typedef geometry::section<robust_box_type, 1> section_type;

        strategy::buffer::piece_type type;
        signed_size_type index;

        signed_size_type left_index; // points to previous piece of same ring
        signed_size_type right_index; // points to next piece of same ring

        // The next two members (1, 2) form together a complete clockwise ring
        // for each piece (with one dupped point)
        // The complete clockwise ring is also included as a robust ring (3)

        // 1: half, part of offsetted_rings
        segment_identifier first_seg_id;
        signed_size_type last_segment_index; // no segment-identifier - it is the same as first_seg_id
        signed_size_type offsetted_count; // part in robust_ring which is part of offsetted ring

#if defined(BOOST_GEOMETRY_BUFFER_USE_HELPER_POINTS)
        // 2: half, not part of offsetted rings - part of robust ring
        std::vector<point_type> helper_points; // 4 points for side, 3 points for join - 0 points for flat-end
#endif
        bool is_flat_start;
        bool is_flat_end;

        bool is_deflated;
        bool is_convex;
        bool is_monotonic_increasing[2]; // 0=x, 1=y
        bool is_monotonic_decreasing[2]; // 0=x, 1=y

        // Monotonic sections of pieces around points
        std::vector<section_type> sections;

        // Robust representations
        // 3: complete ring
        robust_ring_type robust_ring;

        robust_box_type robust_envelope;
        robust_box_type robust_offsetted_envelope;

        robust_point_type robust_center;
        robust_comparable_radius_type robust_min_comparable_radius;
        robust_comparable_radius_type robust_max_comparable_radius;

        piece()
            : type(strategy::buffer::piece_type_unknown)
            , index(-1)
            , left_index(-1)
            , right_index(-1)
            , last_segment_index(-1)
            , offsetted_count(-1)
            , is_flat_start(false)
            , is_flat_end(false)
            , is_deflated(false)
            , is_convex(false)
            , robust_min_comparable_radius(0)
            , robust_max_comparable_radius(0)
        {
            is_monotonic_increasing[0] = false;
            is_monotonic_increasing[1] = false;
            is_monotonic_decreasing[0] = false;
            is_monotonic_decreasing[1] = false;
        }
    };

    struct original_ring
    {
        typedef geometry::sections<robust_box_type, 1> sections_type;

        // Creates an empty instance
        inline original_ring()
            : m_is_interior(false)
            , m_has_interiors(false)
        {}

        inline original_ring(robust_ring_type const& ring,
                bool is_interior, bool has_interiors,
                envelope_strategy_type const& envelope_strategy,
                expand_strategy_type const& expand_strategy)
            : m_ring(ring)
            , m_is_interior(is_interior)
            , m_has_interiors(has_interiors)
        {
            geometry::envelope(m_ring, m_box, envelope_strategy);

            // create monotonic sections in x-dimension
            // The dimension is critical because the direction is later used
            // in the optimization for within checks using winding strategy
            // and this strategy is scanning in x direction.
            typedef boost::mpl::vector_c<std::size_t, 0> dimensions;
            geometry::sectionalize<false, dimensions>(m_ring,
                    detail::no_rescale_policy(), m_sections,
                    envelope_strategy, expand_strategy);
        }

        robust_ring_type m_ring;
        robust_box_type m_box;
        sections_type m_sections;

        bool m_is_interior;
        bool m_has_interiors;
    };

    typedef std::vector<piece> piece_vector_type;

    piece_vector_type m_pieces;
    turn_vector_type m_turns;
    signed_size_type m_first_piece_index;
    bool m_deflate;
    bool m_has_deflated;

    // Offsetted rings, and representations of original ring(s)
    // both indexed by multi_index
    buffered_ring_collection<buffered_ring<Ring> > offsetted_rings;
    std::vector<original_ring> original_rings;

    buffered_ring_collection<Ring> traversed_rings;
    segment_identifier current_segment_id;

    // Specificly for offsetted rings around points
    // but also for large joins with many points
    typedef geometry::sections<robust_box_type, 2> sections_type;
    sections_type monotonic_sections;

    // Define the clusters, mapping cluster_id -> turns
    typedef std::map
        <
            signed_size_type,
            detail::overlay::cluster_info
        > cluster_type;

    cluster_type m_clusters;

    IntersectionStrategy m_intersection_strategy;
    DistanceStrategy m_distance_strategy;
    side_strategy_type m_side_strategy;
    area_strategy_type m_area_strategy;
    envelope_strategy_type m_envelope_strategy;
    expand_strategy_type m_expand_strategy;
    point_in_geometry_strategy_type m_point_in_geometry_strategy;

    robust_area_strategy_type m_robust_area_strategy;
    RobustPolicy const& m_robust_policy;

    buffered_piece_collection(IntersectionStrategy const& intersection_strategy,
                              DistanceStrategy const& distance_strategy,
                              RobustPolicy const& robust_policy)
        : m_first_piece_index(-1)
        , m_deflate(false)
        , m_has_deflated(false)
        , m_intersection_strategy(intersection_strategy)
        , m_distance_strategy(distance_strategy)
        , m_side_strategy(intersection_strategy.get_side_strategy())
        , m_area_strategy(intersection_strategy
            .template get_area_strategy<point_type>())
        , m_envelope_strategy(intersection_strategy.get_envelope_strategy())
        , m_expand_strategy(intersection_strategy.get_expand_strategy())
        , m_point_in_geometry_strategy(intersection_strategy
            .template get_point_in_geometry_strategy<robust_point_type,
                        robust_ring_type>())
        , m_robust_area_strategy(intersection_strategy
            .template get_area_strategy<robust_point_type>())
        , m_robust_policy(robust_policy)
    {}


    inline void classify_turns()
    {
        for (typename boost::range_iterator<turn_vector_type>::type it =
            boost::begin(m_turns); it != boost::end(m_turns); ++it)
        {
            if (it->count_within > 0)
            {
                it->location = inside_buffer;
            }
            if (it->count_within_near_offsetted > 0)
            {
                // Within can have in rare cases a rounding issue. We don't discard this
                // point, so it can be used to continue started rings in traversal. But
                // will never start a new ring from this type of points.
                it->operations[0].enriched.startable = false;
                it->operations[1].enriched.startable = false;
            }
        }
    }

    struct deflate_properties
    {
        bool has_inflated;
        std::size_t count;

        inline deflate_properties()
            : has_inflated(false)
            , count(0u)
        {}
    };

    inline void discard_turns_for_deflate()
    {
        // Deflate cases should have at least 3 points PER deflated original
        // to form a correct triangle

        // But if there are intersections between a deflated ring and another
        // ring, it is all accepted

        // In deflate most turns are i/u by nature, but u/u is also possible

        std::map<signed_size_type, deflate_properties> properties;

        for (typename boost::range_iterator<turn_vector_type const>::type it =
            boost::begin(m_turns); it != boost::end(m_turns); ++it)
        {
            const buffer_turn_info_type& turn = *it;
            if (turn.location == location_ok)
            {
                const buffer_turn_operation_type& op0 = turn.operations[0];
                const buffer_turn_operation_type& op1 = turn.operations[1];

                if (! m_pieces[op0.seg_id.piece_index].is_deflated
                 || ! m_pieces[op1.seg_id.piece_index].is_deflated)
                {
                    properties[op0.seg_id.multi_index].has_inflated = true;
                    properties[op1.seg_id.multi_index].has_inflated = true;
                    continue;
                }

                // It is deflated, update counts
                for (int i = 0; i < 2; i++)
                {
                    const buffer_turn_operation_type& op = turn.operations[i];
                    if (op.operation == detail::overlay::operation_union
                        || op.operation == detail::overlay::operation_continue)
                    {
                        properties[op.seg_id.multi_index].count++;
                    }
                }
            }
        }

        for (typename boost::range_iterator<turn_vector_type>::type it =
            boost::begin(m_turns); it != boost::end(m_turns); ++it)
        {
            buffer_turn_info_type& turn = *it;

            if (turn.location == location_ok)
            {
                const buffer_turn_operation_type& op0 = turn.operations[0];
                const buffer_turn_operation_type& op1 = turn.operations[1];
                signed_size_type const multi0 = op0.seg_id.multi_index;
                signed_size_type const multi1 = op1.seg_id.multi_index;

                if (multi0 == multi1)
                {
                    const deflate_properties& prop =  properties[multi0];

                    // NOTE: Keep brackets around prop.count
                    // avoid gcc-bug "parse error in template argument list"
                    // GCC versions 5.4 and 5.5 (and probably more)
                    if (! prop.has_inflated && (prop.count) < 3)
                    {
                        // Property is not inflated
                        // Not enough points, this might be caused by <float> where
                        // detection turn-in-original failed because of numeric errors
                        turn.location = location_discard;
                    }
                }
                else
                {
                    // Two different (possibly deflated) rings
                }
            }
        }
    }

    inline void check_remaining_points()
    {
        // Check if a turn is inside any of the originals

        typedef turn_in_original_ovelaps_box
            <
                typename IntersectionStrategy::disjoint_point_box_strategy_type
            > turn_in_original_ovelaps_box_type;
        typedef original_ovelaps_box
            <
                typename IntersectionStrategy::disjoint_box_box_strategy_type
            > original_ovelaps_box_type;

        turn_in_original_visitor
            <
                turn_vector_type,
                point_in_geometry_strategy_type
            > visitor(m_turns, m_point_in_geometry_strategy);

        geometry::partition
            <
                robust_box_type,
                include_turn_policy,
                detail::partition::include_all_policy
            >::apply(m_turns, original_rings, visitor,
                     turn_get_box(), turn_in_original_ovelaps_box_type(),
                     original_get_box(), original_ovelaps_box_type());

        bool const deflate = m_distance_strategy.negative();

        for (typename boost::range_iterator<turn_vector_type>::type it =
            boost::begin(m_turns); it != boost::end(m_turns); ++it)
        {
            buffer_turn_info_type& turn = *it;
            if (turn.location == location_ok)
            {
                if (deflate && turn.count_in_original <= 0)
                {
                    // For deflate/negative buffers: it is not in original, discard
                    turn.location = location_discard;
                }
                else if (! deflate && turn.count_in_original > 0)
                {
                    // For inflate: it is in original, discard
                    turn.location = location_discard;
                }
            }
        }

        if (m_has_deflated)
        {
            // Either strategy was negative, or there were interior rings
            discard_turns_for_deflate();
        }
    }

    inline void update_turn_administration()
    {
        // Add rescaled turn points to corresponding pieces
        std::size_t index = 0;
        for (typename boost::range_iterator<turn_vector_type>::type it =
            boost::begin(m_turns); it != boost::end(m_turns); ++it, ++index)
        {
            geometry::recalculate(it->robust_point, it->point, m_robust_policy);
            it->turn_index = index;
        }
    }

    template <std::size_t Dimension>
    static inline void determine_monotonicity(piece& pc,
            robust_point_type const& current,
            robust_point_type const& next)
    {
        if (geometry::get<Dimension>(current) >= geometry::get<Dimension>(next))
        {
            pc.is_monotonic_increasing[Dimension] = false;
        }
        if (geometry::get<Dimension>(current) <= geometry::get<Dimension>(next))
        {
            pc.is_monotonic_decreasing[Dimension] = false;
        }
    }

    inline void determine_properties(piece& pc) const
    {
        pc.is_monotonic_increasing[0] = true;
        pc.is_monotonic_increasing[1] = true;
        pc.is_monotonic_decreasing[0] = true;
        pc.is_monotonic_decreasing[1] = true;

        pc.is_convex = geometry::is_convex(pc.robust_ring, m_side_strategy);

        if (pc.offsetted_count < 2)
        {
            return;
        }

        typename robust_ring_type::const_iterator current = pc.robust_ring.begin();
        typename robust_ring_type::const_iterator next = current + 1;

        for (signed_size_type i = 1; i < pc.offsetted_count; i++)
        {
            determine_monotonicity<0>(pc, *current, *next);
            determine_monotonicity<1>(pc, *current, *next);
            current = next;
            ++next;
        }
    }

    void determine_properties()
    {
        for (typename piece_vector_type::iterator it = boost::begin(m_pieces);
            it != boost::end(m_pieces);
            ++it)
        {
            determine_properties(*it);
        }
    }

    inline void reverse_negative_robust_rings()
    {
        for (typename piece_vector_type::iterator it = boost::begin(m_pieces);
            it != boost::end(m_pieces);
            ++it)
        {
            piece& pc = *it;
            if (geometry::area(pc.robust_ring, m_robust_area_strategy) < 0)
            {
                // Rings can be ccw:
                // - in a concave piece
                // - in a line-buffer with a negative buffer-distance
                std::reverse(pc.robust_ring.begin(), pc.robust_ring.end());
            }
        }
    }

    inline void prepare_buffered_point_piece(piece& pc)
    {
        // create monotonic sections in y-dimension
        typedef boost::mpl::vector_c<std::size_t, 1> dimensions;
        geometry::sectionalize<false, dimensions>(pc.robust_ring,
                detail::no_rescale_policy(), pc.sections,
                m_envelope_strategy, m_expand_strategy);

        // Determine min/max radius
        typedef geometry::model::referring_segment<robust_point_type const>
            robust_segment_type;

        typename robust_ring_type::const_iterator current = pc.robust_ring.begin();
        typename robust_ring_type::const_iterator next = current + 1;

        for (signed_size_type i = 1; i < pc.offsetted_count; i++)
        {
            robust_segment_type s(*current, *next);
            robust_comparable_radius_type const d
                = geometry::comparable_distance(pc.robust_center, s);

            if (i == 1 || d < pc.robust_min_comparable_radius)
            {
                pc.robust_min_comparable_radius = d;
            }
            if (i == 1 || d > pc.robust_max_comparable_radius)
            {
                pc.robust_max_comparable_radius = d;
            }

            current = next;
            ++next;
        }
    }

    inline void prepare_buffered_point_pieces()
    {
        for (typename piece_vector_type::iterator it = boost::begin(m_pieces);
            it != boost::end(m_pieces);
            ++it)
        {
            if (it->type == geometry::strategy::buffer::buffered_point)
            {
                prepare_buffered_point_piece(*it);
            }
        }
    }

    inline void get_turns()
    {
        {
            // Calculate the turns
            piece_turn_visitor
                <
                    piece_vector_type,
                    buffered_ring_collection<buffered_ring<Ring> >,
                    turn_vector_type,
                    IntersectionStrategy,
                    RobustPolicy
                > visitor(m_pieces, offsetted_rings, m_turns,
                          m_intersection_strategy, m_robust_policy);

            typedef detail::section::get_section_box
                <
                    typename IntersectionStrategy::expand_box_strategy_type
                > get_section_box_type;
            typedef detail::section::overlaps_section_box
                <
                    typename IntersectionStrategy::disjoint_box_box_strategy_type
                > overlaps_section_box_type;

            detail::sectionalize::enlarge_sections(monotonic_sections,
                                                   m_envelope_strategy);
            geometry::partition
                <
                    robust_box_type
                >::apply(monotonic_sections, visitor,
                         get_section_box_type(),
                         overlaps_section_box_type());
        }

        update_turn_administration();

        reverse_negative_robust_rings();

        determine_properties();

        prepare_buffered_point_pieces();

        {
            // Check if it is inside any of the pieces
            turn_in_piece_visitor
                <
                    typename geometry::cs_tag<point_type>::type,
                    turn_vector_type, piece_vector_type,
                    DistanceStrategy,
                    point_in_geometry_strategy_type,
                    side_strategy_type
                > visitor(m_turns, m_pieces,
                          m_distance_strategy,
                          m_point_in_geometry_strategy,
                          m_side_strategy);

            typedef turn_ovelaps_box
                <
                    typename IntersectionStrategy::disjoint_point_box_strategy_type
                > turn_ovelaps_box_type;
            typedef piece_ovelaps_box
                <
                    typename IntersectionStrategy::disjoint_box_box_strategy_type
                > piece_ovelaps_box_type;

            geometry::partition
                <
                    robust_box_type
                >::apply(m_turns, m_pieces, visitor,
                         turn_get_box(), turn_ovelaps_box_type(),
                         piece_get_box(), piece_ovelaps_box_type());

        }
    }

    inline void start_new_ring(bool deflate)
    {
        std::size_t const n = offsetted_rings.size();
        current_segment_id.source_index = 0;
        current_segment_id.multi_index = static_cast<signed_size_type>(n);
        current_segment_id.ring_index = -1;
        current_segment_id.segment_index = 0;

        offsetted_rings.resize(n + 1);
        original_rings.resize(n + 1);

        m_first_piece_index = static_cast<signed_size_type>(boost::size(m_pieces));
        m_deflate = deflate;
        if (deflate)
        {
            // Pieces contain either deflated exterior rings, or inflated
            // interior rings which are effectively deflated too
            m_has_deflated = true;
        }
    }

    inline void abort_ring()
    {
        // Remove all created pieces for this ring, sections, last offsetted
        while (! m_pieces.empty()
               && m_pieces.back().first_seg_id.multi_index
               == current_segment_id.multi_index)
        {
            m_pieces.pop_back();
        }

        offsetted_rings.pop_back();
        original_rings.pop_back();

        m_first_piece_index = -1;
    }

    inline void update_last_point(point_type const& p,
            buffered_ring<Ring>& ring)
    {
        // For the first point of a new piece, and there were already
        // points in the offsetted ring, for some piece types the first point
        // is a duplicate of the last point of the previous piece.

        // TODO: disable that, that point should not be added

        // For now, it is made equal because due to numerical instability,
        // it can be a tiny bit off, possibly causing a self-intersection

        BOOST_GEOMETRY_ASSERT(boost::size(m_pieces) > 0);
        if (! ring.empty()
            && current_segment_id.segment_index
                == m_pieces.back().first_seg_id.segment_index)
        {
            ring.back() = p;
        }
    }

    inline void set_piece_center(point_type const& center)
    {
        BOOST_GEOMETRY_ASSERT(! m_pieces.empty());
        geometry::recalculate(m_pieces.back().robust_center, center,
                m_robust_policy);
    }

    inline bool finish_ring(strategy::buffer::result_code code)
    {
        if (code == strategy::buffer::result_error_numerical)
        {
            abort_ring();
            return false;
        }

        if (m_first_piece_index == -1)
        {
            return false;
        }

        // Casted version
        std::size_t const first_piece_index
                = static_cast<std::size_t>(m_first_piece_index);
        signed_size_type const last_piece_index
                = static_cast<signed_size_type>(boost::size(m_pieces)) - 1;

        if (first_piece_index < boost::size(m_pieces))
        {
            // If pieces were added,
            // reassign left-of-first and right-of-last
            geometry::range::at(m_pieces, first_piece_index).left_index
                    = last_piece_index;
            geometry::range::back(m_pieces).right_index = m_first_piece_index;
        }

        buffered_ring<Ring>& added = offsetted_rings.back();
        if (! boost::empty(added))
        {
            // Make sure the closing point is identical (they are calculated
            // separately by different pieces)
            range::back(added) = range::front(added);
        }

        for (std::size_t i = first_piece_index; i < boost::size(m_pieces); i++)
        {
            sectionalize(m_pieces[i], added);
        }

        m_first_piece_index = -1;
        return true;
    }

    template <typename InputRing>
    inline void finish_ring(strategy::buffer::result_code code,
                            InputRing const& input_ring,
                            bool is_interior, bool has_interiors)
    {
        if (! finish_ring(code))
        {
            return;
        }

        if (! input_ring.empty())
        {
            // Assign the ring to the original_ring collection
            // For rescaling, it is recalculated. Without rescaling, it
            // is just assigning (note that this Ring type is the
            // GeometryOut type, which might differ from the input ring type)
            geometry::model::ring<robust_point_type> adapted_ring;

            typedef detail::normalized_view<InputRing const> view_type;
            view_type const view(input_ring);

            for (typename boost::range_iterator<view_type const>::type it =
                boost::begin(view); it != boost::end(view); ++it)
            {
                robust_point_type adapted_point;
                geometry::recalculate(adapted_point, *it, m_robust_policy);
                adapted_ring.push_back(adapted_point);
            }

            original_rings.back()
                = original_ring(adapted_ring,
                    is_interior, has_interiors,
                    m_envelope_strategy, m_expand_strategy);
        }
    }

    inline void set_current_ring_concave()
    {
        BOOST_GEOMETRY_ASSERT(boost::size(offsetted_rings) > 0);
        offsetted_rings.back().has_concave = true;
    }

    inline signed_size_type add_point(point_type const& p)
    {
        BOOST_GEOMETRY_ASSERT(boost::size(offsetted_rings) > 0);

        buffered_ring<Ring>& current_ring = offsetted_rings.back();
        update_last_point(p, current_ring);

        current_segment_id.segment_index++;
        current_ring.push_back(p);
        return static_cast<signed_size_type>(current_ring.size());
    }

    //-------------------------------------------------------------------------

    inline piece& create_piece(strategy::buffer::piece_type type,
            bool decrease_segment_index_by_one)
    {
        if (type == strategy::buffer::buffered_concave)
        {
            offsetted_rings.back().has_concave = true;
        }

        piece pc;
        pc.type = type;
        pc.index = static_cast<signed_size_type>(boost::size(m_pieces));
        pc.is_deflated = m_deflate;

        current_segment_id.piece_index = pc.index;

        pc.first_seg_id = current_segment_id;

        // Assign left/right (for first/last piece per ring they will be re-assigned later)
        pc.left_index = pc.index - 1;
        pc.right_index = pc.index + 1;

        std::size_t const n = boost::size(offsetted_rings.back());
        pc.first_seg_id.segment_index = decrease_segment_index_by_one ? n - 1 : n;
        pc.last_segment_index = pc.first_seg_id.segment_index;

        m_pieces.push_back(pc);
        return m_pieces.back();
    }

    inline void init_rescale_piece(piece& pc, std::size_t helper_points_size)
    {
        if (pc.first_seg_id.segment_index < 0)
        {
            // This indicates an error situation: an earlier piece was empty
            // It currently does not happen
            pc.offsetted_count = 0;
            return;
        }

        BOOST_GEOMETRY_ASSERT(pc.first_seg_id.multi_index >= 0);
        BOOST_GEOMETRY_ASSERT(pc.last_segment_index >= 0);

        pc.offsetted_count = pc.last_segment_index - pc.first_seg_id.segment_index;
        BOOST_GEOMETRY_ASSERT(pc.offsetted_count >= 0);

        pc.robust_ring.reserve(pc.offsetted_count + helper_points_size);

        // Add rescaled offsetted segments
        {
            buffered_ring<Ring> const& ring = offsetted_rings[pc.first_seg_id.multi_index];

            typedef typename boost::range_iterator<const buffered_ring<Ring> >::type it_type;
            for (it_type it = boost::begin(ring) + pc.first_seg_id.segment_index;
                it != boost::begin(ring) + pc.last_segment_index;
                ++it)
            {
                robust_point_type point;
                geometry::recalculate(point, *it, m_robust_policy);
                pc.robust_ring.push_back(point);
            }
        }
    }

    inline void add_helper_point(piece& pc, const point_type& point)
    {
#if defined(BOOST_GEOMETRY_BUFFER_USE_HELPER_POINTS)
        pc.helper_points.push_back(point);
#endif

        robust_point_type rob_point;
        geometry::recalculate(rob_point, point, m_robust_policy);
        pc.robust_ring.push_back(rob_point);
    }

    template <typename Box>
    static inline void enlarge_box(Box& box)
    {
#if defined(BOOST_GEOMETRY_USE_RESCALING)
        // Enlarge the box by 1 pixel, or 1 unit
        detail::buffer::buffer_box(box, 1, box);
#else
        // Enlarge the box just a bit
        detail::buffer::buffer_box(box, 0.001, box);
#endif
    }

    inline void calculate_robust_envelope(piece& pc)
    {
        if (pc.offsetted_count == 0)
        {
            return;
        }

        geometry::envelope(pc.robust_ring, pc.robust_envelope, m_envelope_strategy);

        geometry::assign_inverse(pc.robust_offsetted_envelope);
        for (signed_size_type i = 0; i < pc.offsetted_count; i++)
        {
            geometry::expand(pc.robust_offsetted_envelope, pc.robust_ring[i]);
        }

        // Take roundings into account, enlarge boxes
        enlarge_box(pc.robust_envelope);
        enlarge_box(pc.robust_offsetted_envelope);
    }

    inline void sectionalize(piece const& pc, buffered_ring<Ring> const& ring)
    {
        typedef geometry::detail::sectionalize::sectionalize_part
        <
            point_type,
            boost::mpl::vector_c<std::size_t, 0, 1> // x,y dimension
        > sectionalizer;

        // Create a ring-identifier. The source-index is the piece index
        // The multi_index is as in this collection (the ring), but not used here
        // The ring_index is not used
        ring_identifier const ring_id(pc.index, pc.first_seg_id.multi_index, -1);

        sectionalizer::apply(monotonic_sections,
            boost::begin(ring) + pc.first_seg_id.segment_index,
            boost::begin(ring) + pc.last_segment_index,
            m_robust_policy,
            ring_id, 10);
    }

    inline void finish_piece(piece& pc)
    {
        init_rescale_piece(pc, 0u);
        calculate_robust_envelope(pc);
    }

    inline void finish_piece(piece& pc,
                    const point_type& point1,
                    const point_type& point2,
                    const point_type& point3)
    {
        init_rescale_piece(pc, 3u);
        if (pc.offsetted_count == 0)
        {
            return;
        }

        add_helper_point(pc, point1);
        add_helper_point(pc, point2);
        add_helper_point(pc, point3);
        calculate_robust_envelope(pc);
    }

    inline void finish_piece(piece& pc,
                    const point_type& point1,
                    const point_type& point2,
                    const point_type& point3,
                    const point_type& point4)
    {
        init_rescale_piece(pc, 4u);
        add_helper_point(pc, point1);
        add_helper_point(pc, point2);
        add_helper_point(pc, point3);
        add_helper_point(pc, point4);
        calculate_robust_envelope(pc);
    }

    template <typename Range>
    inline void add_range_to_piece(piece& pc, Range const& range, bool add_front)
    {
        BOOST_GEOMETRY_ASSERT(boost::size(range) != 0u);

        typename Range::const_iterator it = boost::begin(range);

        // If it follows a non-join (so basically the same piece-type) point b1 should be added.
        // There should be two intersections later and it should be discarded.
        // But for now we need it to calculate intersections
        if (add_front)
        {
            add_point(*it);
        }

        for (++it; it != boost::end(range); ++it)
        {
            pc.last_segment_index = add_point(*it);
        }
    }

    inline void add_piece(strategy::buffer::piece_type type, point_type const& p,
            point_type const& b1, point_type const& b2)
    {
        piece& pc = create_piece(type, false);
        add_point(b1);
        pc.last_segment_index = add_point(b2);
        finish_piece(pc, b2, p, b1);
    }

    template <typename Range>
    inline void add_piece(strategy::buffer::piece_type type, Range const& range,
            bool decrease_segment_index_by_one)
    {
        piece& pc = create_piece(type, decrease_segment_index_by_one);

        if (boost::size(range) > 0u)
        {
            add_range_to_piece(pc, range, offsetted_rings.back().empty());
        }
        finish_piece(pc);
    }

    template <typename Range>
    inline void add_piece(strategy::buffer::piece_type type,
            point_type const& p, Range const& range)
    {
        piece& pc = create_piece(type, true);

        if (boost::size(range) > 0u)
        {
            add_range_to_piece(pc, range, offsetted_rings.back().empty());
            finish_piece(pc, range.back(), p, range.front());
        }
        else
        {
            finish_piece(pc);
        }
    }

    template <typename Range>
    inline void add_side_piece(point_type const& p1, point_type const& p2,
            Range const& range, bool first)
    {
        BOOST_GEOMETRY_ASSERT(boost::size(range) >= 2u);

        piece& pc = create_piece(strategy::buffer::buffered_segment, ! first);
        add_range_to_piece(pc, range, first);
        finish_piece(pc, range.back(), p2, p1, range.front());
    }

    template <typename EndcapStrategy, typename Range>
    inline void add_endcap(EndcapStrategy const& strategy, Range const& range,
            point_type const& end_point)
    {
        boost::ignore_unused(strategy);

        if (range.empty())
        {
            return;
        }
        strategy::buffer::piece_type pt = strategy.get_piece_type();
        if (pt == strategy::buffer::buffered_flat_end)
        {
            // It is flat, should just be added, without helper segments
            add_piece(pt, range, true);
        }
        else
        {
            // Normal case, it has an "inside", helper segments should be added
            add_piece(pt, end_point, range);
        }
    }

    inline void mark_flat_start()
    {
        if (! m_pieces.empty())
        {
            piece& back = m_pieces.back();
            back.is_flat_start = true;
        }
    }

    inline void mark_flat_end()
    {
        if (! m_pieces.empty())
        {
            piece& back = m_pieces.back();
            back.is_flat_end = true;
        }
    }

    //-------------------------------------------------------------------------

    inline void enrich()
    {
        enrich_intersection_points<false, false, overlay_buffer>(m_turns,
            m_clusters, offsetted_rings, offsetted_rings,
            m_robust_policy,
            m_intersection_strategy);
    }

    // Discards all rings which do have not-OK intersection points only.
    // Those can never be traversed and should not be part of the output.
    inline void discard_rings()
    {
        for (typename boost::range_iterator<turn_vector_type const>::type it =
            boost::begin(m_turns); it != boost::end(m_turns); ++it)
        {
            if (it->location != location_ok)
            {
                offsetted_rings[it->operations[0].seg_id.multi_index].has_discarded_intersections = true;
                offsetted_rings[it->operations[1].seg_id.multi_index].has_discarded_intersections = true;
            }
            else
            {
                offsetted_rings[it->operations[0].seg_id.multi_index].has_accepted_intersections = true;
                offsetted_rings[it->operations[1].seg_id.multi_index].has_accepted_intersections = true;
            }
        }
    }

    inline bool point_coveredby_original(point_type const& point)
    {
        typedef typename IntersectionStrategy::disjoint_point_box_strategy_type d_pb_strategy_type;

        robust_point_type any_point;
        geometry::recalculate(any_point, point, m_robust_policy);

        signed_size_type count_in_original = 0;

        // Check of the robust point of this outputted ring is in
        // any of the robust original rings
        // This can go quadratic if the input has many rings, and there
        // are many untouched deflated rings around
        for (typename std::vector<original_ring>::const_iterator it
            = original_rings.begin();
            it != original_rings.end();
            ++it)
        {
            original_ring const& original = *it;
            if (original.m_ring.empty())
            {
                continue;
            }
            if (detail::disjoint::disjoint_point_box(any_point,
                                                     original.m_box,
                                                     d_pb_strategy_type()))
            {
                continue;
            }

            int const geometry_code
                = detail::within::point_in_geometry(any_point,
                    original.m_ring, m_point_in_geometry_strategy);

            if (geometry_code == -1)
            {
                // Outside, continue
                continue;
            }

            // Apply for possibly nested interior rings
            if (original.m_is_interior)
            {
                count_in_original--;
            }
            else if (original.m_has_interiors)
            {
                count_in_original++;
            }
            else
            {
                // Exterior ring without interior rings
                return true;
            }
        }
        return count_in_original > 0;
    }

    // For a deflate, all rings around inner rings which are untouched
    // (no intersections/turns) and which are OUTSIDE the original should
    // be discarded
    inline void discard_nonintersecting_deflated_rings()
    {
        for(typename buffered_ring_collection<buffered_ring<Ring> >::iterator it
            = boost::begin(offsetted_rings);
            it != boost::end(offsetted_rings);
            ++it)
        {
            buffered_ring<Ring>& ring = *it;
            if (! ring.has_intersections()
                && boost::size(ring) > 0u
                && geometry::area(ring, m_area_strategy) < 0)
            {
                if (! point_coveredby_original(geometry::range::front(ring)))
                {
                    ring.is_untouched_outside_original = true;
                }
            }
        }
    }

    inline void block_turns()
    {
        for (typename boost::range_iterator<turn_vector_type>::type it =
            boost::begin(m_turns); it != boost::end(m_turns); ++it)
        {
            buffer_turn_info_type& turn = *it;
            if (turn.location != location_ok)
            {
                // Discard this turn (don't set it to blocked to avoid colocated
                // clusters being discarded afterwards
                turn.discarded = true;
            }
        }
    }

    inline void traverse()
    {
        typedef detail::overlay::traverse
            <
                false, false,
                buffered_ring_collection<buffered_ring<Ring> >,
                buffered_ring_collection<buffered_ring<Ring > >,
                overlay_buffer,
                backtrack_for_buffer
            > traverser;
        std::map<ring_identifier, overlay::ring_turn_info> turn_info_per_ring;

        traversed_rings.clear();
        buffer_overlay_visitor visitor;
        traverser::apply(offsetted_rings, offsetted_rings,
                        m_intersection_strategy, m_robust_policy,
                        m_turns, traversed_rings,
                        turn_info_per_ring,
                        m_clusters, visitor);
    }

    inline void reverse()
    {
        for(typename buffered_ring_collection<buffered_ring<Ring> >::iterator it = boost::begin(offsetted_rings);
            it != boost::end(offsetted_rings);
            ++it)
        {
            if (! it->has_intersections())
            {
                std::reverse(it->begin(), it->end());
            }
        }
        for (typename boost::range_iterator<buffered_ring_collection<Ring> >::type
                it = boost::begin(traversed_rings);
                it != boost::end(traversed_rings);
                ++it)
        {
            std::reverse(it->begin(), it->end());
        }

    }

    template <typename GeometryOutput, typename OutputIterator>
    inline OutputIterator assign(OutputIterator out) const
    {
        typedef detail::overlay::ring_properties<point_type, area_result_type> properties;

        std::map<ring_identifier, properties> selected;

        // Select all rings which do not have any self-intersection
        // Inner rings, for deflate, which do not have intersections, and
        // which are outside originals, are skipped
        // (other ones should be traversed)
        signed_size_type index = 0;
        for(typename buffered_ring_collection<buffered_ring<Ring> >::const_iterator it = boost::begin(offsetted_rings);
            it != boost::end(offsetted_rings);
            ++it, ++index)
        {
            if (! it->has_intersections()
                && ! it->is_untouched_outside_original)
            {
                properties p = properties(*it, m_area_strategy);
                if (p.valid)
                {
                    ring_identifier id(0, index, -1);
                    selected[id] = p;
                }
            }
        }

        // Select all created rings
        index = 0;
        for (typename boost::range_iterator<buffered_ring_collection<Ring> const>::type
                it = boost::begin(traversed_rings);
                it != boost::end(traversed_rings);
                ++it, ++index)
        {
            properties p = properties(*it, m_area_strategy);
            if (p.valid)
            {
                ring_identifier id(2, index, -1);
                selected[id] = p;
            }
        }

        detail::overlay::assign_parents<overlay_buffer>(offsetted_rings, traversed_rings,
                selected, m_intersection_strategy);
        return detail::overlay::add_rings<GeometryOutput>(selected, offsetted_rings, traversed_rings, out,
                                                          m_area_strategy);
    }

};


}} // namespace detail::buffer
#endif // DOXYGEN_NO_DETAIL


}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_ALGORITHMS_DETAIL_BUFFER_BUFFERED_PIECE_COLLECTION_HPP
