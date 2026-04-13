#pragma once

#include <cstdint>

namespace midi {
    // Status byte constants
    namespace Status {
        // Channel Voice messages (high nibble only)
        // Low nibble is the channel (0-15)
        constexpr uint8_t noteOff = 0x80;
        constexpr uint8_t noteOn = 0x90;
        constexpr uint8_t polyKeyPressure = 0xA0;
        constexpr uint8_t controlChange = 0xB0;
        constexpr uint8_t programChange = 0xC0;
        constexpr uint8_t channelPressure = 0xD0;
        constexpr uint8_t pitchBend = 0xE0;

        // System messages have no channel number (0xF0-0xFF)
        // System Common. 0xF4 and 0xF5 are undefined
        constexpr uint8_t sysExStart = 0xF0;
        constexpr uint8_t quarterFrame = 0xF1;
        constexpr uint8_t songPosition = 0xF2;
        constexpr uint8_t songSelect = 0xF3;
        constexpr uint8_t tuneRequest = 0xF6;
        constexpr uint8_t sysExEnd = 0xF7;

        // System Real Time. 0xF9 and 0xFD are undefined
        constexpr uint8_t timingClock = 0xF8;
        constexpr uint8_t start = 0xFA;
        constexpr uint8_t cont = 0xFB; // 'continue' is a C++ keyword
        constexpr uint8_t stop = 0xFC;
        constexpr uint8_t activeSensing = 0xFE;
        constexpr uint8_t systemReset = 0xFF;
    } // namespace Status

    // Channel Mode. Controller numbers in the 120-127 range
    namespace ChannelMode {
        constexpr uint8_t allSoundOff = 120;
        constexpr uint8_t resetAllControllers = 121;
        constexpr uint8_t localControl = 122;
        constexpr uint8_t allNotesOff = 123;
        constexpr uint8_t omniModeOff = 124;
        constexpr uint8_t omniModeOn = 125;
        constexpr uint8_t monoModeOn = 126;
        constexpr uint8_t polyModeOn = 127;
    } // namespace ChannelMode

    // MidiEvent: fixed-size, representing a 1-3 byte MIDI message.
    // No SysEx or MetaEvent support
    class MidiEvent {
    public:
        // Channel Voice factory methods
        // Channel parameters are 1-16 (not 0-15)
        static MidiEvent noteOn(int channel, int note, int velocity);
        static MidiEvent noteOff(int channel, int note, int velocity = 0);
        static MidiEvent polyKeyPressure(int channel, int note, int pressure);
        static MidiEvent controlChange(int channel, int cc, int value);
        static MidiEvent programChange(int channel, int program);
        static MidiEvent channelPressure(int channel, int pressure);
        static MidiEvent pitchBend(int channel, int value14bit); // 0-16383, 8192 = center

        // Channel Mode factory methods (Controller 120-127)
        static MidiEvent allSoundOff(int channel);
        static MidiEvent resetAllControllers(int channel);
        static MidiEvent allNotesOff(int channel);

        // Factory methods for System Common and System Real Time messages are not defined here

        // From raw bytes — for parsing incoming MIDI streams
        // Returns a default-constructed (invalid) event for:
        //      1. Message type mismatch with number of bytes
        //      2. invalid status byte
        //      3. SysEx is considered as invalid. Handle separately if needed.
        // Use isValid() on the result to check for failure.
        static MidiEvent fromRaw(const uint8_t* data, int numBytes, double timestamp = 0.0);

        // Creates an invalid event of size 0.
        MidiEvent() noexcept : timestamp(0.0), status(0), data1(0), data2(0), size(0) {}

        // Boolean Classification methods

        // Check validity
        [[nodiscard]] bool isValid() const noexcept {
            return size > 0;
        }

        // Channel Voice
        [[nodiscard]] bool isNoteOn(bool returnTrueForVelocity0 = false) const noexcept;
        [[nodiscard]] bool isNoteOff(bool returnTrueForNoteOnVelocity0 = true) const noexcept;
        [[nodiscard]] bool isNoteOnOrOff() const noexcept;
        [[nodiscard]] bool isPolyKeyPressure() const noexcept;
        [[nodiscard]] bool isController() const noexcept;
        [[nodiscard]] bool isControllerOfType(int controllerNumber) const noexcept;
        [[nodiscard]] bool isProgramChange() const noexcept;
        [[nodiscard]] bool isChannelPressure() const noexcept;
        [[nodiscard]] bool isPitchBend() const noexcept;

        // Channel Mode
        [[nodiscard]] bool isAllSoundOff() const noexcept;
        [[nodiscard]] bool isResetAllControllers() const noexcept;
        [[nodiscard]] bool isAllNotesOff() const noexcept;
        [[nodiscard]] bool isSustainPedalOn() const noexcept;  // CC 64, value >= 64
        [[nodiscard]] bool isSustainPedalOff() const noexcept; // CC 64, value < 64

        // Classifiers for System Common and System Real Time messages are not defined here

        // Catch-all
        [[nodiscard]] bool isChannelMessage() const noexcept; // 0x80-0xEF
        [[nodiscard]] bool isSystemMessage() const noexcept;  // 0xF0-0xFF

        // Getters
        // Channel Voice
        [[nodiscard]] int getChannel() const noexcept;         // 1-16; 0 for system messages
        [[nodiscard]] int getNoteNumber() const noexcept;
        [[nodiscard]] int getVelocity() const noexcept;        // 0-127
        [[nodiscard]] float getFloatVelocity() const noexcept; // 0.0-1.0
        [[nodiscard]] int getControllerNumber() const noexcept;
        [[nodiscard]] int getControllerValue() const noexcept;
        [[nodiscard]] int getProgramChangeNumber() const noexcept;
        [[nodiscard]] int getChannelPressureValue() const noexcept;
        [[nodiscard]] int getPolyKeyPressureValue() const noexcept;
        [[nodiscard]] int getPitchBendValue() const noexcept; // 0-16383

        // System Common Omitted

        // Raw access
        [[nodiscard]] const uint8_t* getRawData() const noexcept {
            return &status;
        }
        [[nodiscard]] uint8_t getStatusByte() const noexcept {
            return status;
        }
        [[nodiscard]] int getRawDataSize() const noexcept {
            return size;
        }

        // Timing
        [[nodiscard]] double getTimestamp() const noexcept {
            return timestamp;
        }
        void setTimestamp(double t) noexcept {
            timestamp = t;
        }
        void addToTimestamp(double delta) noexcept {
            timestamp += delta;
        }

        // Setters
        void setChannel(int newChannel) noexcept;     // 1-16; no-op for system
        void setNoteNumber(int newNote) noexcept;
        void setVelocity(int newVelocity) noexcept;   // 0-127
        void setVelocity(float newVelocity) noexcept; // 0.0-1.0
        void multiplyVelocity(float scale) noexcept;
        void setControllerValue(int value) noexcept;
        void setPitchBendValue(int value14bit) noexcept;

        // Utility
        [[nodiscard]] static int messageLengthFromStatus(uint8_t statusByte) noexcept;

    private:
        MidiEvent(uint8_t s, uint8_t d1, uint8_t d2, uint8_t sz, double ts) noexcept :
            timestamp(ts), status(s), data1(d1), data2(d2), size(sz) {}

        // Layout: 8 + 1 + 1 + 1 + 1 = 12 bytes, padded to 16(?).
        // &status is a valid 3-byte buffer
        double timestamp;
        uint8_t status;
        uint8_t data1;
        uint8_t data2;
        uint8_t size;
    };
} // namespace midi
