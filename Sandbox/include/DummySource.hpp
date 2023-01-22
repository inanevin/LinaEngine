#include "World/Core/Component.hpp"
#include "Reflection/ClassReflection.hpp"
#include "Data/Streams.hpp"

class DummySource : public Lina::Component
{
public:
    int MovementSpeed = 5;

    virtual void SaveToStream(Lina::OStream& stream)
    {
        stream << MovementSpeed;
    };
    virtual void LoadFromStream(Lina::IStream& stream)
    {

        stream >> MovementSpeed;
    };

private:
    LINA_REFLECTION_ACCESS(DummySource);

    float m_rotateSpeed;

    // Inherited via Component
    virtual Lina::TypeID GetTID()
    {
        return Lina::GetTypeID<DummySource>();
    }
};

// LINA_REFLECTCLASS_BEGIN(DummySource, "DummySrc", "", true)
// LINA_REFLECTCLASS_FIELD(DummySource, MovementSpeed, "FieldTitle", "Int", "", "Hehe", "")
// LINA_REFLECTCLASS_FIELD(DummySource, m_rotateSpeed, "FieldTitle", "Int", "", "Hehe", "")
// LINA_REFLECTCLASS_END(DummySource)
