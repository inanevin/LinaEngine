#ifndef CSTDOUTSTREAM_HPP
#define CSTDOUTSTREAM_HPP

#include <ostream>
#include <cstdint>

#include "7zip/IStream.h"
#include "Common/MyCom.h"

namespace bit7z {
    using std::ostream;

    class CStdOutStream : public IOutStream, public CMyUnknownImp {
        public:
            explicit CStdOutStream( std::ostream& outputStream );

            virtual ~CStdOutStream();

            MY_UNKNOWN_IMP1( IOutStream )

            // IOutStream
            STDMETHOD( Write )( void const* data, uint32_t size, uint32_t* processedSize );
            STDMETHOD( Seek )( int64_t offset, uint32_t seekOrigin, uint64_t* newPosition );
            STDMETHOD( SetSize )( uint64_t newSize );

        private:
            ostream& mOutputStream;
    };
}

#endif // CSTDOUTSTREAM_HPP
