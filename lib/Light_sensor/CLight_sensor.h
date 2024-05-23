#include <SpaIot.h> // Include the SpaIot library. This library contains the Event class that we are going to extend.

// Define a new class CLight that extends the Event class from the SpaIot library.
class CLight : public SpaIot::Event {
public:
    // Define a new enumeration Type inside the CLight class. This enumeration will contain the types of events that instances of CLight can represent.
    enum Type {
        // Define a new event type LightMode. The value of LightMode is calculated as one less than the value of AnyEvent in the Event class from the SpaIot library.
        LightMode = SpaIot::Event::AnyEvent - 1,
        // Define a new event type LuxValue. The value of LuxValue is calculated as one less than the value of LightMode.
        LuxValue = LightMode - 1
    };

    // Define the constructor for the CLight class. The constructor takes two parameters: the type of the event and its value.
    // The type parameter has a default value of LightMode, and the value parameter has a default value of 0.
    // The constructor initializes the base Event class with the given type and value. The type is cast to SpaIot::Event::Type because the base class expects this type.
    CLight(Type type = Type::LightMode, uint16_t value = 0) : SpaIot::Event(static_cast<SpaIot::Event::Type>(type), value) {}

    // Define a new constructor for the CLight class that takes a lux value as a parameter.
    // The constructor initializes the base Event class with the LuxValue event type and the given lux value.
    CLight(uint16_t lux) : SpaIot::Event(static_cast<SpaIot::Event::Type>(Type::LuxValue), lux) {}
};