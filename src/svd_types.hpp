#pragma once

#include "inja_wrapper.hpp"

struct BitRange {
    std::uint64_t start;
    std::uint64_t stop;
};

enum class DataType { u8, u16, u32, u64 };

NLOHMANN_JSON_SERIALIZE_ENUM(
  DataType,
  {
    { DataType::u8,  "u8"},
    {DataType::u16, "u16"},
    {DataType::u32, "u32"},
    {DataType::u64, "u64"}
})

enum class RepeatType {
    normal,
    cluster,
};

NLOHMANN_JSON_SERIALIZE_ENUM(
  RepeatType,
  {
    { RepeatType::normal,  "normal"},
    {RepeatType::cluster, "cluster"}
})

enum class FieldType { normal, enum_ };

NLOHMANN_JSON_SERIALIZE_ENUM(
  FieldType,
  {
    {FieldType::normal, "normal"},
    { FieldType::enum_,   "enum"}
})

enum class Access { readOnly, writeOnly, readWrite, writeOnce, readWriteOnce };

NLOHMANN_JSON_SERIALIZE_ENUM(
  Access,
  {
    {     Access::readOnly,      "readOnly"},
    {    Access::writeOnly,     "writeOnly"},
    {    Access::readWrite,     "readWrite"},
    {    Access::writeOnce,     "writeOnce"},
    {Access::readWriteOnce, "readWriteOnce"},
})

enum class ModifiedWriteValues {
    empty,
    oneToClear,
    oneToSet,
    oneToToggle,
    zeroToClear,
    zeroToSet,
    zeroToToggle,
    clear,
    set,
    modify
};

NLOHMANN_JSON_SERIALIZE_ENUM(ModifiedWriteValues,
                             {
                                 {ModifiedWriteValues::empty, "empty"},
                                 {ModifiedWriteValues::oneToClear, "oneToClear"},
                                 {ModifiedWriteValues::oneToSet, "oneToSet"},
                                 {ModifiedWriteValues::oneToToggle, "oneToToggle"},
                                 {ModifiedWriteValues::zeroToClear, "zeroToClear"},
                                 {ModifiedWriteValues::zeroToSet, "zeroToSet"},
                                 {ModifiedWriteValues::zeroToToggle, "zeroToToggle"},
                                 {ModifiedWriteValues::clear, "clear"},
                                 {ModifiedWriteValues::set, "set"},
                                 {ModifiedWriteValues::modify, "modify"},
                             })
enum class ReadAction { empty, clear, set, modify, modifyExternal };

NLOHMANN_JSON_SERIALIZE_ENUM(ReadAction, {
                                             {ReadAction::empty, ""},
                                             {ReadAction::clear, "clear"},
                                             {ReadAction::set, "set"},
                                             {ReadAction::modify, "modify"},
                                             {ReadAction::modifyExternal, "modifyExternal"},
                                         })

struct Value {
    std::string    name;
    std::string    description;
    std::uint64_t  value;
    constexpr bool operator==(Value const& other) const = default;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Value, name, description, value)
};

struct Field {
    std::string         name;
    std::string         description;
    FieldType           type;
    RepeatType          repType;
    std::uint64_t       dim;
    std::uint64_t       dimIncrement;
    std::uint64_t       resetValue;
    std::uint64_t       startBit;
    std::uint64_t       stopBit;
    DataType            dataType;
    Access              access;
    ModifiedWriteValues modifiedWriteValues;
    ReadAction          readAction;
    std::vector<Value>  values;
    constexpr bool      similar(Field const& other) const {
        return name == other.name && type == other.type && repType == other.repType &&
               dim == other.dim && dimIncrement == other.dimIncrement &&
               startBit == other.startBit && stopBit == other.stopBit &&
               dataType == other.dataType && access == other.access &&
               modifiedWriteValues == other.modifiedWriteValues && readAction == other.readAction &&
               values == other.values;
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Field, name, description, type, repType, dim, dimIncrement,
                                   resetValue, startBit, stopBit, dataType, access,
                                   modifiedWriteValues, readAction, values)
};

struct Register {
    std::string   name;
    std::string   description;
    RepeatType    type;
    std::uint64_t dim;
    std::uint64_t dimIncrement;
    DataType      dataType;

    std::uint64_t      addressOffset;
    std::uint64_t      resetValue;
    std::uint64_t      zeroMask;
    std::uint64_t      oneMask;
    std::vector<Field> fields;
    bool               similar(Register const& other) const {
        return name == other.name && type == other.type && dim == other.dim &&
               dimIncrement == other.dimIncrement && dataType == other.dataType &&
               addressOffset == other.addressOffset && zeroMask == other.zeroMask &&
               oneMask == other.oneMask &&
               std::equal(fields.begin(), fields.end(), other.fields.begin(), other.fields.end(),
                                        [](auto const& l, auto const& r) { return l.similar(r); });
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Register, name, description, type, dim, dimIncrement, dataType,
                                   addressOffset, resetValue, zeroMask, oneMask, fields)
};

struct RegisterGroup {
    std::string           name;
    std::string           description;
    std::uint64_t         dim;
    std::uint64_t         dimIncrement;
    std::uint64_t         addressOffset;
    std::vector<Register> registers;
    constexpr bool        similar(RegisterGroup const& other) const {
        return name == other.name && dim == other.dim && dimIncrement == other.dimIncrement &&
               addressOffset == other.addressOffset &&
               std::equal(registers.begin(), registers.end(), other.registers.begin(),
                                 other.registers.end(),
                                 [](auto const& l, auto const& r) { return l.similar(r); });
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RegisterGroup, name, description, dim, dimIncrement,
                                   addressOffset, registers)
};

struct AddressType {
    std::uint64_t  index;
    std::uint64_t  address;
    constexpr bool operator==(AddressType const&) const = default;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AddressType, index, address)
};

struct Interrupt {
    std::string   name;
    std::string   description;
    std::uint64_t value;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Interrupt, name, description, value)
};

struct Peripheral {
    std::string                name;
    std::string                description;
    RepeatType                 type;
    DataType                   addressType;
    std::vector<Interrupt>     interrupt;
    std::vector<Register>      registers;
    std::vector<RegisterGroup> registerGroups;
    std::vector<AddressType>   baseAddresses;
    constexpr bool             similar(Peripheral const& other) const {
        return type == other.type && addressType == other.addressType &&
               std::equal(registers.begin(), registers.end(), other.registers.begin(),
                                      other.registers.end(),
                                      [](auto const& l, auto const& r) { return l.similar(r); }) &&
               std::equal(registerGroups.begin(), registerGroups.end(),
                                      other.registerGroups.begin(), other.registerGroups.end(),
                                      [](auto const& l, auto const& r) { return l.similar(r); });
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Peripheral, name, description, type, baseAddresses, addressType,
                                   registers, registerGroups)
};

struct Chip {
    std::string             name;
    std::string             cpu_name;
    bool                    mpu{false};
    bool                    fpu{false};
    uint64_t                nvic_bits{8};
    std::string             description;
    std::vector<Peripheral> peripherals;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Chip, name, cpu_name, mpu, fpu, nvic_bits, description,
                                   peripherals)
};
