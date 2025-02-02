#pragma once

#include "unicorn/core.hpp"
#include "unicorn/property-values.hpp"
#include <cstring>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace Unicorn {

    // Constants

    constexpr char32_t last_ascii_char                = 0x7f;            // Highest ASCII code point
    constexpr char32_t last_latin1_char               = 0xff;            // Highest ISO 8859 code point
    constexpr char32_t line_separator_char            = 0x2028;          // Line separator
    constexpr char32_t paragraph_separator_char       = 0x2029;          // Paragraph separator
    constexpr char32_t first_surrogate_char           = 0xd800;          // First UTF-16 surrogate code
    constexpr char32_t first_high_surrogate_char      = 0xd800;          // First UTF-16 high surrogate code
    constexpr char32_t last_high_surrogate_char       = 0xdbff;          // Last UTF-16 high surrogate code
    constexpr char32_t first_low_surrogate_char       = 0xdc00;          // First UTF-16 low surrogate code
    constexpr char32_t last_low_surrogate_char        = 0xdfff;          // Last UTF-16 low surrogate code
    constexpr char32_t last_surrogate_char            = 0xdfff;          // Last UTF-16 surrogate code
    constexpr char32_t first_private_use_char         = 0xe000;          // Beginning of BMP private use area
    constexpr char32_t last_private_use_char          = 0xf8ff;          // End of BMP private use area
    constexpr char32_t first_noncharacter             = 0xfdd0;          // Beginning of reserved noncharacter block
    constexpr char32_t last_noncharacter              = 0xfdef;          // End of reserved noncharacter block
    constexpr char32_t byte_order_mark                = 0xfeff;          // Byte order mark
    constexpr char32_t replacement_char               = 0xfffd;          // Unicode replacement character
    constexpr char32_t last_bmp_char                  = 0xffff;          // End of basic multilingual plane
    constexpr char32_t first_private_use_a_char       = 0xf0000;         // Beginning of supplementary private use area A
    constexpr char32_t last_private_use_a_char        = 0xffffd;         // End of supplementary private use area A
    constexpr char32_t first_private_use_b_char       = 0x100000;        // Beginning of supplementary private use area B
    constexpr char32_t last_private_use_b_char        = 0x10fffd;        // End of supplementary private use area B
    constexpr char32_t last_unicode_char              = 0x10ffff;        // Highest possible Unicode code point
    constexpr const char* utf8_bom                    = "\xef\xbb\xbf";  // Byte order mark (U+FEFF) in UTF-8
    constexpr const char* utf8_replacement            = "\xef\xbf\xbd";  // Unicode replacement character (U+FFFD) in UTF-8
    constexpr size_t max_case_decomposition           = 3;               // Maximum length of a full case mapping
    constexpr size_t max_canonical_decomposition      = 2;               // Maximum length of a canonical decomposition
    constexpr size_t max_compatibility_decomposition  = 18;              // Maximum length of a compatibility decomposition

    // Basic character functions

    inline u8string char_as_hex(char32_t c) { return "U+" + ascii_uppercase(hex(c, 4)); }
    constexpr bool char_is_digit(char32_t c) noexcept { return c >= U'0' && c <= U'9'; }
    constexpr bool char_is_xdigit(char32_t c) noexcept
        { return (c >= U'0' && c <= U'9') || (c >= U'A' && c <= U'F') || (c >= U'a' && c <= U'f'); }
    constexpr bool char_is_ascii(char32_t c) noexcept { return c <= last_ascii_char; }
    constexpr bool char_is_latin1(char32_t c) noexcept { return c <= last_latin1_char; }
    constexpr bool char_is_surrogate(char32_t c) noexcept { return c >= first_surrogate_char && c <= last_surrogate_char; }
    constexpr bool char_is_bmp(char32_t c) noexcept { return c <= last_bmp_char && ! char_is_surrogate(c); }
    constexpr bool char_is_astral(char32_t c) noexcept { return c > last_bmp_char && c <= last_unicode_char; }
    constexpr bool char_is_unicode(char32_t c) noexcept { return c <= last_unicode_char && ! char_is_surrogate(c); }
    constexpr bool char_is_high_surrogate(char32_t c) noexcept { return c >= first_high_surrogate_char && c <= last_high_surrogate_char; }
    constexpr bool char_is_low_surrogate(char32_t c) noexcept { return c >= first_low_surrogate_char && c <= last_low_surrogate_char; }
    constexpr bool char_is_noncharacter(char32_t c) noexcept
        { return (c >= first_noncharacter && c <= last_noncharacter) || (c & 0xfffe) == 0xfffe; }
    constexpr bool char_is_private_use(char32_t c) noexcept
        { return (c >= first_private_use_char && c <= last_private_use_char)
            || (c >= first_private_use_a_char && c <= last_private_use_a_char)
            || (c >= first_private_use_b_char && c <= last_private_use_b_char); }
    template <typename C> constexpr uint32_t char_to_uint(C c) noexcept { return std::make_unsigned_t<C>(c); }

    // General category

    inline u8string decode_gc(uint16_t cat) { return {char((cat >> 8) & 0xff), char(cat & 0xff)}; }
    constexpr uint16_t encode_gc(char c1, char c2) noexcept { return uint16_t((uint16_t(uint8_t(c1)) << 8) + uint8_t(c2)); }
    constexpr uint16_t encode_gc(const char* cat) noexcept { return cat && *cat ? encode_gc(cat[0], cat[1]) : 0; }
    inline uint16_t encode_gc(const u8string& cat) noexcept { return encode_gc(cat.data()); }
    const char* gc_name(uint16_t cat) noexcept;

    namespace GC {

        enum GC: uint16_t {
            Cc = encode_gc("Cc"),  // Other: Control
            Cf = encode_gc("Cf"),  // Other: Format
            Cn = encode_gc("Cn"),  // Other: Unassigned
            Co = encode_gc("Co"),  // Other: Private use
            Cs = encode_gc("Cs"),  // Other: Surrogate
            Ll = encode_gc("Ll"),  // Letter: Lowercase letter
            Lm = encode_gc("Lm"),  // Letter: Modifier letter
            Lo = encode_gc("Lo"),  // Letter: Other letter
            Lt = encode_gc("Lt"),  // Letter: Titlecase letter
            Lu = encode_gc("Lu"),  // Letter: Uppercase letter
            Mc = encode_gc("Mc"),  // Mark: Spacing mark
            Me = encode_gc("Me"),  // Mark: Enclosing mark
            Mn = encode_gc("Mn"),  // Mark: Nonspacing mark
            Nd = encode_gc("Nd"),  // Number: Decimal number
            Nl = encode_gc("Nl"),  // Number: Letter number
            No = encode_gc("No"),  // Number: Other number
            Pc = encode_gc("Pc"),  // Punctuation: Connector punctuation
            Pd = encode_gc("Pd"),  // Punctuation: Dash punctuation
            Pe = encode_gc("Pe"),  // Punctuation: Close punctuation
            Pf = encode_gc("Pf"),  // Punctuation: Final punctuation
            Pi = encode_gc("Pi"),  // Punctuation: Initial punctuation
            Po = encode_gc("Po"),  // Punctuation: Other punctuation
            Ps = encode_gc("Ps"),  // Punctuation: Open punctuation
            Sc = encode_gc("Sc"),  // Symbol: Currency symbol
            Sk = encode_gc("Sk"),  // Symbol: Modifier symbol
            Sm = encode_gc("Sm"),  // Symbol: Math symbol
            So = encode_gc("So"),  // Symbol: Other symbol
            Zl = encode_gc("Zl"),  // Separator: Line separator
            Zp = encode_gc("Zp"),  // Separator: Paragraph separator
            Zs = encode_gc("Zs"),  // Separator: Space separator
        };

        inline std::ostream& operator<<(std::ostream& o, GC cat) { return o << decode_gc(cat); }

    }

    uint16_t char_general_category(char32_t c) noexcept;
    inline char char_primary_category(char32_t c) noexcept { return char(char_general_category(c) >> 8); }
    inline bool char_is_alphanumeric(char32_t c) noexcept { auto g = char_primary_category(c); return g == 'L' || g == 'N'; }
    inline bool char_is_control(char32_t c) noexcept { return char_general_category(c) == GC::Cc; }
    inline bool char_is_format(char32_t c) noexcept { return char_general_category(c) == GC::Cf; }
    inline bool char_is_letter(char32_t c) noexcept { return char_primary_category(c) == 'L'; }
    inline bool char_is_mark(char32_t c) noexcept { return char_primary_category(c) == 'M'; }
    inline bool char_is_number(char32_t c) noexcept { return char_primary_category(c) == 'N'; }
    inline bool char_is_punctuation(char32_t c) noexcept { return char_primary_category(c) == 'P'; }
    inline bool char_is_symbol(char32_t c) noexcept { return char_primary_category(c) == 'S'; }
    inline bool char_is_separator(char32_t c) noexcept { return char_primary_category(c) == 'Z'; }

    std::function<bool(char32_t)> gc_predicate(uint16_t cat);
    std::function<bool(char32_t)> gc_predicate(const u8string& cat);
    std::function<bool(char32_t)> gc_predicate(const char* cat);

    // Boolean properties

    inline bool char_is_assigned(char32_t c) noexcept { return char_general_category(c) != GC::Cn; }
    inline bool char_is_unassigned(char32_t c) noexcept { return char_general_category(c) == GC::Cn; }
    bool char_is_white_space(char32_t c) noexcept;
    inline bool char_is_line_break(char32_t c) noexcept { return c == U'\n' || c == U'\v' || c == U'\f' || c == U'\r'
        || c == 0x85 || c == line_separator_char || c == paragraph_separator_char; }
    inline bool char_is_inline_space(char32_t c) noexcept { return char_is_white_space(c) && ! char_is_line_break(c); }
    bool char_is_id_start(char32_t c) noexcept;
    bool char_is_id_nonstart(char32_t c) noexcept;
    inline bool char_is_id_continue(char32_t c) noexcept { return char_is_id_start(c) || char_is_id_nonstart(c); }
    bool char_is_xid_start(char32_t c) noexcept;
    bool char_is_xid_nonstart(char32_t c) noexcept;
    inline bool char_is_xid_continue(char32_t c) noexcept { return char_is_xid_start(c) || char_is_xid_nonstart(c); }
    bool char_is_pattern_syntax(char32_t c) noexcept;
    bool char_is_pattern_white_space(char32_t c) noexcept;
    bool char_is_default_ignorable(char32_t c) noexcept;
    bool char_is_soft_dotted(char32_t c) noexcept;

    // Bidirectional properties

    Bidi_Class bidi_class(char32_t c) noexcept;
    bool char_is_bidi_mirrored(char32_t c) noexcept;
    char32_t bidi_mirroring_glyph(char32_t c) noexcept;
    char32_t bidi_paired_bracket(char32_t c) noexcept;
    char bidi_paired_bracket_type(char32_t c) noexcept;

    // Block properties

    struct BlockInfo {
        u8string name;
        char32_t first;
        char32_t last;
    };

    u8string char_block(char32_t c);
    const vector<BlockInfo>& unicode_block_list();

    // Case folding properties

    bool char_is_uppercase(char32_t c) noexcept;
    bool char_is_lowercase(char32_t c) noexcept;
    inline bool char_is_titlecase(char32_t c) noexcept { return char_general_category(c) == GC::Lt; }
    bool char_is_cased(char32_t c) noexcept;
    bool char_is_case_ignorable(char32_t c) noexcept;
    char32_t char_to_simple_uppercase(char32_t c) noexcept;
    char32_t char_to_simple_lowercase(char32_t c) noexcept;
    char32_t char_to_simple_titlecase(char32_t c) noexcept;
    char32_t char_to_simple_casefold(char32_t c) noexcept;
    size_t char_to_full_uppercase(char32_t c, char32_t* dst) noexcept;
    size_t char_to_full_lowercase(char32_t c, char32_t* dst) noexcept;
    size_t char_to_full_titlecase(char32_t c, char32_t* dst) noexcept;
    size_t char_to_full_casefold(char32_t c, char32_t* dst) noexcept;

    // Character names

    constexpr uint32_t cn_control  = 1u << 0;
    constexpr uint32_t cn_label    = 1u << 1;
    constexpr uint32_t cn_lower    = 1u << 2;
    constexpr uint32_t cn_prefix   = 1u << 3;
    constexpr uint32_t cn_update   = 1u << 4;

    u8string char_name(char32_t c, uint32_t flags = 0);

    // Decomposition properties

    int combining_class(char32_t c) noexcept;
    char32_t canonical_composition(char32_t u1, char32_t u2) noexcept;
    size_t canonical_decomposition(char32_t c, char32_t* dst) noexcept;
    size_t compatibility_decomposition(char32_t c, char32_t* dst) noexcept;

    // Enumeration properties

    std::ostream& operator<<(std::ostream& o, Bidi_Class x);
    std::ostream& operator<<(std::ostream& o, East_Asian_Width x);
    std::ostream& operator<<(std::ostream& o, Grapheme_Cluster_Break x);
    std::ostream& operator<<(std::ostream& o, Hangul_Syllable_Type x);
    std::ostream& operator<<(std::ostream& o, Indic_Positional_Category x);
    std::ostream& operator<<(std::ostream& o, Indic_Syllabic_Category x);
    std::ostream& operator<<(std::ostream& o, Joining_Group x);
    std::ostream& operator<<(std::ostream& o, Joining_Type x);
    std::ostream& operator<<(std::ostream& o, Line_Break x);
    std::ostream& operator<<(std::ostream& o, Numeric_Type x);
    std::ostream& operator<<(std::ostream& o, Sentence_Break x);
    std::ostream& operator<<(std::ostream& o, Word_Break x);
    East_Asian_Width east_asian_width(char32_t c) noexcept;
    Grapheme_Cluster_Break grapheme_cluster_break(char32_t c) noexcept;
    Hangul_Syllable_Type hangul_syllable_type(char32_t c) noexcept;
    Indic_Positional_Category indic_positional_category(char32_t c) noexcept;
    Indic_Syllabic_Category indic_syllabic_category(char32_t c) noexcept;
    Joining_Group joining_group(char32_t c) noexcept;
    Joining_Type joining_type(char32_t c) noexcept;
    Line_Break line_break(char32_t c) noexcept;
    Numeric_Type numeric_type(char32_t c) noexcept;
    Sentence_Break sentence_break(char32_t c) noexcept;
    Word_Break word_break(char32_t c) noexcept;

    // Numeric properties

    std::pair<long long, long long> numeric_value(char32_t c);

    // Script properties

    u8string char_script(char32_t c);
    vector<u8string> char_script_list(char32_t c);
    u8string script_name(const u8string& abbr);

}
