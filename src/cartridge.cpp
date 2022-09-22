#include "cartridge.h"

#include "log.h"

#include <memory>
#include <optional>

// NOTE: this implementation is for the file format INES

std::optional<Cartridge> Cartridge::from_file(const char* file_name)
{
    FILE* file = fopen(file_name, "rb");
    if (!file) {
        info_message("fopen failed: {}", strerror(errno));
        return std::nullopt;
    }

    return from_file(file);
}

std::optional<Cartridge> Cartridge::from_file(FILE* file)
{
    auto buffer       = std::make_unique<uint8_t[]>(max_size);
    size_t bytes_read = fread(buffer.get(), 1, max_size, file);
    if (ferror(file)) {
        info_message("fread failed: {}", errno);
        return std::nullopt;
    }
    if (!feof(file)) {
        info_message("fread no eof hit after {} bytes", bytes_read);
        return std::nullopt;
    }

    DEBUG_LOG("fread got eof after {} bytes", bytes_read);
    return from_memory(std::span(buffer.get(), bytes_read));
}

std::optional<Cartridge> Cartridge::from_memory(std::span<uint8_t> buf)
{
    if (buf.size() < 16 || buf[0] != 'N' || buf[1] != 'E' || buf[2] != 'S' || buf[3] != 0x1A) {
        info_message("rom has invalid signature");
        return std::nullopt;
    }

    std::optional<Cartridge> cart(std::in_place);
    memcpy(&cart->m_Header, buf.data(), sizeof(Header));

    info_message("program rom banks: {}", cart->m_Header.prg_bank_count);
    info_message("video/character rom banks: {}", cart->m_Header.chr_bank_count);
    info_message("control byte 1: {:08b}", cart->m_Header.control_byte_1);
    info_message("control byte 2: {:08b}", cart->m_Header.control_byte_2);
    info_message("ram banks: {}", cart->m_Header.ram_banks);
    info_message("mapper number: {}", cart->m_Header.get_mapper_number());

    if (cart->m_Header.control_byte_1 & (1 << 1)) {
        info_message("has battery backed ram, not supported yet");
        return std::nullopt;
    }
    if (cart->m_Header.control_byte_1 & (1 << 2)) {
        info_message("has trainer, not supported yet");
        return std::nullopt;
    }

    const uint32_t program_size = cart->m_Header.prg_bank_count * 16 * 1024;
    buf                         = buf.subspan(16);
    cart->m_PrgData             = std::make_unique<uint8_t[]>(program_size);
    if (buf.size() < program_size) {
        info_message("program memory ({}) claimed to be larger than what exists ({})", program_size, buf.size());
        return std::nullopt;
    }
    memcpy(cart->m_PrgData.get(), buf.data(), program_size);

    const uint32_t video_size = cart->m_Header.chr_bank_count * 8192;
    buf                       = buf.subspan(program_size);
    cart->m_VideoData         = std::make_unique<uint8_t[]>(video_size);
    if (buf.size() < video_size) {
        info_message("video memory ({}) claimed to be larger than what exists ({})", video_size, buf.size());
        return std::nullopt;
    }
    memcpy(cart->m_VideoData.get(), buf.data(), video_size);

    return cart;
}

uint8_t Cartridge::Header::get_mapper_number() const
{
    return control_byte_2 | (control_byte_1 >> 4);
}

std::span<const uint8_t> Cartridge::get_program_data() const
{
    return std::span<const uint8_t>(m_PrgData.get(), m_Header.prg_bank_count * 16 * 1024);
}

std::span<const uint8_t> Cartridge::get_video_data() const
{
    return std::span<const uint8_t>(m_VideoData.get(), m_Header.chr_bank_count * 8192);
}
