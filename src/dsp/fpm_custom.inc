union upper_parts
{
    struct
    {
        uhalf h2, h3;
    };
    struct
    {
        ubase u_no_sign : (width - 1), u_sign : 1;
    };
    ubase au;
    base u;
};

union middle_parts
{
    struct
    {
        uhalf h1, h2;
    };
    struct
    {
        ubase m_no_sign : (width - 1), m_sign : 1;
    };
    ubase am;
    base m;
};

union lower_parts
{
    struct
    {
        uhalf h0, h1;
    };
    struct
    {
        ubase l_no_sign : (width - 1), l_sign : 1;
    };
    base l;
    ubase al;
};

union ext_parts
{
    struct
    {
        uhalf h0, h1, h2, h3;
    };
    struct
    {
        ubase l_no_sign : (width - 1), l_sign : 1, u_no_sign : (width - 1), u_sign : 1;
    };
    struct
    {
        ubase alpad : (width / 2), am : (width), aupad : (width / 2);
    };
    struct
    {
        ubase lpad : (width / 2), m_no_sign : (width - 1), m_sign : 1, upad : (width / 2);
    };
    struct
    {
        ubase al, au;
    };
    struct
    {
        uext e_no_sign : (width * 2 - 1), e_sign : 1;
    };
    uext ae;
    ext e = 0;
};

// addition and subtraction can be done just using the x_from_y functions

// multiplication
// using half bases, the multiplication of two ext is defined as follows:
// a0 b0 + a0 b1 x + a0 b2 x^2 + a0 b3 x^3 + a1 b0 x + a1 b1 x^2 + a1 b2 x^3 + a1 b3 x^4 + a2 b0 x^2 + a2 b1 x^3 + a2 b2 x^4 + a2 b3 x^5 + a3 b0 x^3 + a3 b1 x^4 + a3 b2 x^5 + a3 b3 x^6
//(a_0 b_0) +
//(a_0 b_1 + a_1 b_0) x +
//(a_0 b_2 + a_1 b_1 + a_2 b_0) x^2 +
//(a_0 b_3 + a_1 b_2 + a_2 b_1 + a_3 b_0) x^3 +
//(a_1 b_3 + a_2 b_2 + a_3 b_1) x^4 +
//(a_2 b_3 + a_3 b_2) x^5 +
//(a_3 b_3) x^6

inline constexpr ubase block_0(ubase a0, ubase b0)
{
    return a0 * b0;
}

inline constexpr ubase block_1(ubase a0, ubase a1, ubase b0, ubase b1)
{
    return a0 * b1 + a1 * b0;
}

inline constexpr ubase block_2(ubase a0, ubase a1, ubase a2, ubase b0, ubase b1, ubase b2)
{
    return a0 * b2 + a1 * b1 + a2 * b0;
}

inline constexpr ubase block_3(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
{
    return a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0;
}

inline constexpr ubase block_4(ubase a1, ubase a2, ubase a3, ubase b1, ubase b2, ubase b3)
{
    return a1 * b3 + a2 * b2 + a3 * b1;
}

inline constexpr ubase block_5(ubase a2, ubase a3, ubase b2, ubase b3)
{
    return a2 * b3 + a3 * b2;
}

inline constexpr ubase block_6(ubase a3, ubase b3)
{
    return a3 * b3;
}

inline constexpr uext e_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
{
    // const ubase blk_0 = block_0(a0, b0);
    // const ubase blk_1 = block_1(a0, a1, b0, b1);
    const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
    const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
    const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
    const ubase blk_5 = block_5(a2, a3, b2, b3);
    // const ubase blk_6 = block_6(a3, b3);

    return (((((((uext)blk_5 << width / 2) + blk_4) << width / 2) + blk_3) << width / 2) + blk_2) & e_max;
}

inline constexpr ubase u_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
{
    // const ubase blk_0 = block_0(a0, b0);
    // const ubase blk_1 = block_1(a0, a1, b0, b1);
    // const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
    // const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
    const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
    const ubase blk_5 = block_5(a2, a3, b2, b3);
    // const ubase blk_6 = block_6(a3, b3);

    return ((blk_5 << width / 2) + blk_4) & u_max;
}

inline constexpr ubase m_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
{
    // const ubase blk_0 = block_0(a0, b0);
    // const ubase blk_1 = block_1(a0, a1, b0, b1);
    // const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
    const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
    const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
    // const ubase blk_5 = block_5(a2, a3, b2, b3);
    // const ubase blk_6 = block_6(a3, b3);

    return ((blk_4 << width / 2) + blk_3) & m_max;
}

inline constexpr ubase al_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
{ // CAREFUL FOR THE SHIFT
    // const ubase blk_0 = block_0(a0, b0);
    // const ubase blk_1 = block_1(a0, a1, b0, b1);
    const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
    const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
    // const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
    // const ubase blk_5 = block_5(a2, a3, b2, b3);
    // const ubase blk_6 = block_6(a3, b3);

    return (((blk_3 << width / 2) + blk_2));
}

inline constexpr ubase l_mul(ubase a0, ubase a1, ubase a2, ubase a3, ubase b0, ubase b1, ubase b2, ubase b3)
{ // CAREFUL FOR THE SHIFT
    // const ubase blk_0 = block_0(a0, b0);
    // const ubase blk_1 = block_1(a0, a1, b0, b1);
    const ubase blk_2 = block_2(a0, a1, a2, b0, b1, b2);
    const ubase blk_3 = block_3(a0, a1, a2, a3, b0, b1, b2, b3);
    // const ubase blk_4 = block_4(a1, a2, a3, b1, b2, b3);
    // const ubase blk_5 = block_5(a2, a3, b2, b3);
    // const ubase blk_6 = block_6(a3, b3);

    return (((blk_3 << width / 2) + blk_2) / 2) & l_max;
}

// disable narrowing conversion warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"

inline constexpr ext e_mul_ee(ext e1, ext e2)
{
    return e1 * e2;

    const ext_parts a = {.e = abs(e1)};
    const ext_parts b = {.e = abs(e2)};

    return e_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
}

inline constexpr ext e_mul_uu(base u1, base u2)
{
    return (ext)u1 * u2;

    // const upper_parts a = { .au = abs(u1) };
    // const upper_parts b = { .au = abs(u2) };

    // return e_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
}

inline constexpr ext e_mul_um(base u, base m)
{
    const upper_parts a = {.au = abs(u)};
    const middle_parts b = {.am = abs(m)};

    return e_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
}

inline constexpr ext e_mul_ul(base u, base l)
{ // CAREFUL FOR THE SHIFT
    const upper_parts a = {.au = abs(u)};
    const lower_parts b = {.al = abs(l) * 2};

    return e_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
}

inline constexpr ext e_mul_ual(base u, ubase al)
{ // CAREFUL FOR THE SHIFT
    const upper_parts a = {.au = abs(u)};
    const lower_parts b = {.al = al};

    return e_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ? -1 : 1);
}

inline constexpr ext e_mul_mm(base m1, base m2)
{
    const middle_parts a = {.am = abs(m1)};
    const middle_parts b = {.am = abs(m2)};

    return e_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
}

inline constexpr ext e_mul_ml(base m, base l)
{ // CAREFUL FOR THE SHIFT
    const middle_parts a = {.am = abs(m)};
    const lower_parts b = {.al = abs(l) * 2};

    return e_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
}

inline constexpr ext e_mul_mal(base m, ubase al)
{ // CAREFUL FOR THE SHIFT
    const middle_parts a = {.am = abs(m)};
    const lower_parts b = {.al = al};

    return e_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ? -1 : 1);
}

inline constexpr ext e_mul_ll(base l1, base l2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = abs(l1) * 2};
    const lower_parts b = {.al = abs(l2) * 2};

    return e_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
}

inline constexpr ext e_mul_lal(base l1, ubase al2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = abs(l1) * 2};
    const lower_parts b = {.al = al2};

    return e_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ? -1 : 1);
}

inline constexpr ext e_mul_alal(ubase al1, ubase al2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = al1};
    const lower_parts b = {.al = al2};

    return e_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0);
}

inline constexpr base u_mul_ee(base e1, base e2)
{
    const ext_parts a = {.e = abs(e1)};
    const ext_parts b = {.e = abs(e2)};

    return u_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
}

inline constexpr base u_mul_uu(base u1, base u2)
{
    const upper_parts a = {.au = abs(u1)};
    const upper_parts b = {.au = abs(u2)};

    return u_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
}

inline constexpr base u_mul_um(base u, base m)
{
    const upper_parts a = {.au = abs(u)};
    const middle_parts b = {.am = abs(m)};

    return u_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
}

inline constexpr base u_mul_ul(base u, base l)
{ // CAREFUL FOR THE SHIFT
    const upper_parts a = {.au = abs(u)};
    const lower_parts b = {.al = abs(l) * 2};

    return u_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
}

inline constexpr base u_mul_ual(base u, ubase al)
{ // CAREFUL FOR THE SHIFT
    const upper_parts a = {.au = abs(u)};
    const lower_parts b = {.al = al};

    return u_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ? -1 : 1);
}

inline constexpr base u_mul_mm(base m1, base m2)
{
    const middle_parts a = {.am = abs(m1)};
    const middle_parts b = {.am = abs(m2)};

    return u_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
}

inline constexpr base u_mul_ml(base m, base l)
{ // CAREFUL FOR THE SHIFT
    const middle_parts a = {.am = abs(m)};
    const lower_parts b = {.al = abs(l) * 2};

    return u_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
}

inline constexpr base u_mul_mal(base m, ubase al)
{ // CAREFUL FOR THE SHIFT
    const middle_parts a = {.am = abs(m)};
    const lower_parts b = {.al = al};

    return u_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ? -1 : 1);
}

inline constexpr base u_mul_ll(base l1, base l2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = abs(l1) * 2};
    const lower_parts b = {.al = abs(l2) * 2};

    return u_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
}

inline constexpr base u_mul_lal(base l1, ubase al2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = abs(l1) * 2};
    const lower_parts b = {.al = al2};

    return u_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ? -1 : 1);
}

inline constexpr base u_mul_alal(ubase al1, ubase al2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = al1};
    const lower_parts b = {.al = al2};

    return u_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0);
}

inline constexpr base m_mul_ee(base e1, base e2)
{
    const ext_parts a = {.ae = abs(e1)};
    const ext_parts b = {.ae = abs(e2)};

    return m_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
}

inline constexpr base m_mul_uu(base u1, base u2)
{
    const upper_parts a = {.au = abs(u1)};
    const upper_parts b = {.au = abs(u2)};

    return m_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
}

inline constexpr base m_mul_um(base u, base m)
{
    const upper_parts a = {.au = abs(u)};
    const middle_parts b = {.am = abs(m)};

    return m_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
}

inline constexpr base m_mul_ul(base u, base l)
{ // CAREFUL FOR THE SHIFT
    const upper_parts a = {.au = abs(u)};
    const lower_parts b = {.al = abs(l) * 2};

    return m_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
}

inline constexpr base m_mul_ual(base u, ubase al)
{ // CAREFUL FOR THE SHIFT
    const upper_parts a = {.au = abs(u)};
    const lower_parts b = {.al = al};

    return m_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ? -1 : 1);
}

inline constexpr base m_mul_mm(base m1, base m2)
{
    const middle_parts a = {.am = abs(m1)};
    const middle_parts b = {.am = abs(m2)};

    return m_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
}

inline constexpr base m_mul_ml(base m, base l)
{ // CAREFUL FOR THE SHIFT
    const middle_parts a = {.am = abs(m)};
    const lower_parts b = {.al = abs(l) * 2};

    return m_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
}

inline constexpr base m_mul_mal(base m, ubase al)
{ // CAREFUL FOR THE SHIFT
    const middle_parts a = {.am = abs(m)};
    const lower_parts b = {.al = al};

    return m_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ? -1 : 1);
}

inline constexpr base m_mul_ll(base l1, base l2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = abs(l1) * 2};
    const lower_parts b = {.al = abs(l2) * 2};

    return m_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
}

inline constexpr base m_mul_lal(base l1, ubase al2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = abs(l1) * 2};
    const lower_parts b = {.al = al2};

    return m_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ? -1 : 1);
}

inline constexpr base m_mul_alal(ubase al1, ubase al2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = al1};
    const lower_parts b = {.al = al2};

    return m_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0);
}

inline constexpr base l_mul_ee(base e1, base e2)
{
    const ext_parts a = {.ae = abs(e1)};
    const ext_parts b = {.ae = abs(e2)};

    return l_mul(a.h0, a.h1, a.h2, a.h3, b.h0, b.h1, b.h2, b.h3) * ((e1 < 0) ^ (e2 < 0) ? -1 : 1);
}

inline constexpr base l_mul_uu(base u1, base u2)
{
    const upper_parts a = {.au = abs(u1)};
    const upper_parts b = {.au = abs(u2)};

    return l_mul(0, 0, a.h2, a.h3, 0, 0, b.h2, b.h3) * ((u1 < 0) ^ (u2 < 0) ? -1 : 1);
}

inline constexpr base l_mul_um(base u, base m)
{
    const upper_parts a = {.au = abs(u)};
    const middle_parts b = {.am = abs(m)};

    return l_mul(0, 0, a.h2, a.h3, 0, b.h1, b.h2, 0) * ((u < 0) ^ (m < 0) ? -1 : 1);
}

inline constexpr base l_mul_ul(base u, base l)
{ // CAREFUL FOR THE SHIFT
    const upper_parts a = {.au = abs(u)};
    const lower_parts b = {.al = abs(l) * 2};

    return l_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ^ (l < 0) ? -1 : 1);
}

inline constexpr base l_mul_ual(base u, ubase al)
{ // CAREFUL FOR THE SHIFT
    const upper_parts a = {.au = abs(u)};
    const lower_parts b = {.al = al};

    return l_mul(0, 0, a.h2, a.h3, b.h0, b.h1, 0, 0) * ((u < 0) ? -1 : 1);
}

inline constexpr base l_mul_mm(base m1, base m2)
{
    const middle_parts a = {.am = abs(m1)};
    const middle_parts b = {.am = abs(m2)};

    return l_mul(0, a.h1, a.h2, 0, 0, b.h1, b.h2, 0) * ((m1 < 0) ^ (m2 < 0) ? -1 : 1);
}

inline constexpr base l_mul_ml(base m, base l)
{ // CAREFUL FOR THE SHIFT
    const middle_parts a = {.am = abs(m)};
    const lower_parts b = {.al = abs(l) * 2};

    return l_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ^ (l < 0) ? -1 : 1);
}

inline constexpr base l_mul_mal(base m, ubase al)
{ // CAREFUL FOR THE SHIFT
    const middle_parts a = {.am = abs(m)};
    const lower_parts b = {.al = al};

    return l_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0) * ((m < 0) ? -1 : 1);
}

inline constexpr ubase al_mul_mal(base m, ubase al)
{ // CAREFUL FOR THE SHIFT
    const middle_parts a = {.am = abs(m)};
    const lower_parts b = {.al = al};

    return al_mul(0, a.h1, a.h2, 0, b.h0, b.h1, 0, 0);
}

inline constexpr base l_mul_ll(base l1, base l2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = abs(l1) * 2};
    const lower_parts b = {.al = abs(l2) * 2};

    return l_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ^ (l2 < 0) ? -1 : 1);
}

inline constexpr base l_mul_lal(base l1, ubase al2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = abs(l1) * 2};
    const lower_parts b = {.al = al2};

    return l_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0) * ((l1 < 0) ? -1 : 1);
}

inline constexpr base l_mul_alal(ubase al1, ubase al2)
{ // CAREFUL FOR THE SHIFT
    const lower_parts a = {.al = al1};
    const lower_parts b = {.al = al2};

    return l_mul(a.h0, a.h1, 0, 0, b.h0, b.h1, 0, 0);
}
