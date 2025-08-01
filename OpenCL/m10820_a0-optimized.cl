/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#define NEW_SIMD_CODE

#ifdef KERNEL_STATIC
#include M2S(INCLUDE_PATH/inc_vendor.h)
#include M2S(INCLUDE_PATH/inc_types.h)
#include M2S(INCLUDE_PATH/inc_platform.cl)
#include M2S(INCLUDE_PATH/inc_common.cl)
#include M2S(INCLUDE_PATH/inc_rp_optimized.h)
#include M2S(INCLUDE_PATH/inc_rp_optimized.cl)
#include M2S(INCLUDE_PATH/inc_simd.cl)
#include M2S(INCLUDE_PATH/inc_hash_sha384.cl)
#endif

DECLSPEC void sha384_transform_intern (PRIVATE_AS const u32x *w0, PRIVATE_AS const u32x *w1, PRIVATE_AS const u32x *w2, PRIVATE_AS const u32x *w3, PRIVATE_AS u64x *digest)
{
  u64x w0_t = hl32_to_64 (w0[0], w0[1]);
  u64x w1_t = hl32_to_64 (w0[2], w0[3]);
  u64x w2_t = hl32_to_64 (w1[0], w1[1]);
  u64x w3_t = hl32_to_64 (w1[2], w1[3]);
  u64x w4_t = hl32_to_64 (w2[0], w2[1]);
  u64x w5_t = hl32_to_64 (w2[2], w2[3]);
  u64x w6_t = hl32_to_64 (w3[0], w3[1]);
  u64x w7_t = 0;
  u64x w8_t = 0;
  u64x w9_t = 0;
  u64x wa_t = 0;
  u64x wb_t = 0;
  u64x wc_t = 0;
  u64x wd_t = 0;
  u64x we_t = 0;
  u64x wf_t = hl32_to_64 (w3[2], w3[3]);

  u64x a = digest[0];
  u64x b = digest[1];
  u64x c = digest[2];
  u64x d = digest[3];
  u64x e = digest[4];
  u64x f = digest[5];
  u64x g = digest[6];
  u64x h = digest[7];

                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, a, b, c, d, e, f, g, h, w0_t, SHA512C00);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, h, a, b, c, d, e, f, g, w1_t, SHA512C01);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, g, h, a, b, c, d, e, f, w2_t, SHA512C02);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, f, g, h, a, b, c, d, e, w3_t, SHA512C03);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, e, f, g, h, a, b, c, d, w4_t, SHA512C04);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, d, e, f, g, h, a, b, c, w5_t, SHA512C05);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, c, d, e, f, g, h, a, b, w6_t, SHA512C06);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, b, c, d, e, f, g, h, a, w7_t, SHA512C07);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, a, b, c, d, e, f, g, h, w8_t, SHA512C08);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, h, a, b, c, d, e, f, g, w9_t, SHA512C09);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, g, h, a, b, c, d, e, f, wa_t, SHA512C0a);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, f, g, h, a, b, c, d, e, wb_t, SHA512C0b);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, e, f, g, h, a, b, c, d, wc_t, SHA512C0c);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, d, e, f, g, h, a, b, c, wd_t, SHA512C0d);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, c, d, e, f, g, h, a, b, we_t, SHA512C0e);
                                                 SHA384_STEP (SHA384_F0o, SHA384_F1o, b, c, d, e, f, g, h, a, wf_t, SHA512C0f);
  w0_t = SHA384_EXPAND (we_t, w9_t, w1_t, w0_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, a, b, c, d, e, f, g, h, w0_t, SHA512C10);
  w1_t = SHA384_EXPAND (wf_t, wa_t, w2_t, w1_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, h, a, b, c, d, e, f, g, w1_t, SHA512C11);
  w2_t = SHA384_EXPAND (w0_t, wb_t, w3_t, w2_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, g, h, a, b, c, d, e, f, w2_t, SHA512C12);
  w3_t = SHA384_EXPAND (w1_t, wc_t, w4_t, w3_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, f, g, h, a, b, c, d, e, w3_t, SHA512C13);
  w4_t = SHA384_EXPAND (w2_t, wd_t, w5_t, w4_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, e, f, g, h, a, b, c, d, w4_t, SHA512C14);
  w5_t = SHA384_EXPAND (w3_t, we_t, w6_t, w5_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, d, e, f, g, h, a, b, c, w5_t, SHA512C15);
  w6_t = SHA384_EXPAND (w4_t, wf_t, w7_t, w6_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, c, d, e, f, g, h, a, b, w6_t, SHA512C16);
  w7_t = SHA384_EXPAND (w5_t, w0_t, w8_t, w7_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, b, c, d, e, f, g, h, a, w7_t, SHA512C17);
  w8_t = SHA384_EXPAND (w6_t, w1_t, w9_t, w8_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, a, b, c, d, e, f, g, h, w8_t, SHA512C18);
  w9_t = SHA384_EXPAND (w7_t, w2_t, wa_t, w9_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, h, a, b, c, d, e, f, g, w9_t, SHA512C19);
  wa_t = SHA384_EXPAND (w8_t, w3_t, wb_t, wa_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, g, h, a, b, c, d, e, f, wa_t, SHA512C1a);
  wb_t = SHA384_EXPAND (w9_t, w4_t, wc_t, wb_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, f, g, h, a, b, c, d, e, wb_t, SHA512C1b);
  wc_t = SHA384_EXPAND (wa_t, w5_t, wd_t, wc_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, e, f, g, h, a, b, c, d, wc_t, SHA512C1c);
  wd_t = SHA384_EXPAND (wb_t, w6_t, we_t, wd_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, d, e, f, g, h, a, b, c, wd_t, SHA512C1d);
  we_t = SHA384_EXPAND (wc_t, w7_t, wf_t, we_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, c, d, e, f, g, h, a, b, we_t, SHA512C1e);
  wf_t = SHA384_EXPAND (wd_t, w8_t, w0_t, wf_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, b, c, d, e, f, g, h, a, wf_t, SHA512C1f);
  w0_t = SHA384_EXPAND (we_t, w9_t, w1_t, w0_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, a, b, c, d, e, f, g, h, w0_t, SHA512C20);
  w1_t = SHA384_EXPAND (wf_t, wa_t, w2_t, w1_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, h, a, b, c, d, e, f, g, w1_t, SHA512C21);
  w2_t = SHA384_EXPAND (w0_t, wb_t, w3_t, w2_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, g, h, a, b, c, d, e, f, w2_t, SHA512C22);
  w3_t = SHA384_EXPAND (w1_t, wc_t, w4_t, w3_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, f, g, h, a, b, c, d, e, w3_t, SHA512C23);
  w4_t = SHA384_EXPAND (w2_t, wd_t, w5_t, w4_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, e, f, g, h, a, b, c, d, w4_t, SHA512C24);
  w5_t = SHA384_EXPAND (w3_t, we_t, w6_t, w5_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, d, e, f, g, h, a, b, c, w5_t, SHA512C25);
  w6_t = SHA384_EXPAND (w4_t, wf_t, w7_t, w6_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, c, d, e, f, g, h, a, b, w6_t, SHA512C26);
  w7_t = SHA384_EXPAND (w5_t, w0_t, w8_t, w7_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, b, c, d, e, f, g, h, a, w7_t, SHA512C27);
  w8_t = SHA384_EXPAND (w6_t, w1_t, w9_t, w8_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, a, b, c, d, e, f, g, h, w8_t, SHA512C28);
  w9_t = SHA384_EXPAND (w7_t, w2_t, wa_t, w9_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, h, a, b, c, d, e, f, g, w9_t, SHA512C29);
  wa_t = SHA384_EXPAND (w8_t, w3_t, wb_t, wa_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, g, h, a, b, c, d, e, f, wa_t, SHA512C2a);
  wb_t = SHA384_EXPAND (w9_t, w4_t, wc_t, wb_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, f, g, h, a, b, c, d, e, wb_t, SHA512C2b);
  wc_t = SHA384_EXPAND (wa_t, w5_t, wd_t, wc_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, e, f, g, h, a, b, c, d, wc_t, SHA512C2c);
  wd_t = SHA384_EXPAND (wb_t, w6_t, we_t, wd_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, d, e, f, g, h, a, b, c, wd_t, SHA512C2d);
  we_t = SHA384_EXPAND (wc_t, w7_t, wf_t, we_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, c, d, e, f, g, h, a, b, we_t, SHA512C2e);
  wf_t = SHA384_EXPAND (wd_t, w8_t, w0_t, wf_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, b, c, d, e, f, g, h, a, wf_t, SHA512C2f);
  w0_t = SHA384_EXPAND (we_t, w9_t, w1_t, w0_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, a, b, c, d, e, f, g, h, w0_t, SHA512C30);
  w1_t = SHA384_EXPAND (wf_t, wa_t, w2_t, w1_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, h, a, b, c, d, e, f, g, w1_t, SHA512C31);
  w2_t = SHA384_EXPAND (w0_t, wb_t, w3_t, w2_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, g, h, a, b, c, d, e, f, w2_t, SHA512C32);
  w3_t = SHA384_EXPAND (w1_t, wc_t, w4_t, w3_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, f, g, h, a, b, c, d, e, w3_t, SHA512C33);
  w4_t = SHA384_EXPAND (w2_t, wd_t, w5_t, w4_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, e, f, g, h, a, b, c, d, w4_t, SHA512C34);
  w5_t = SHA384_EXPAND (w3_t, we_t, w6_t, w5_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, d, e, f, g, h, a, b, c, w5_t, SHA512C35);
  w6_t = SHA384_EXPAND (w4_t, wf_t, w7_t, w6_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, c, d, e, f, g, h, a, b, w6_t, SHA512C36);
  w7_t = SHA384_EXPAND (w5_t, w0_t, w8_t, w7_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, b, c, d, e, f, g, h, a, w7_t, SHA512C37);
  w8_t = SHA384_EXPAND (w6_t, w1_t, w9_t, w8_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, a, b, c, d, e, f, g, h, w8_t, SHA512C38);
  w9_t = SHA384_EXPAND (w7_t, w2_t, wa_t, w9_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, h, a, b, c, d, e, f, g, w9_t, SHA512C39);
  wa_t = SHA384_EXPAND (w8_t, w3_t, wb_t, wa_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, g, h, a, b, c, d, e, f, wa_t, SHA512C3a);
  wb_t = SHA384_EXPAND (w9_t, w4_t, wc_t, wb_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, f, g, h, a, b, c, d, e, wb_t, SHA512C3b);
  wc_t = SHA384_EXPAND (wa_t, w5_t, wd_t, wc_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, e, f, g, h, a, b, c, d, wc_t, SHA512C3c);
  wd_t = SHA384_EXPAND (wb_t, w6_t, we_t, wd_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, d, e, f, g, h, a, b, c, wd_t, SHA512C3d);
  we_t = SHA384_EXPAND (wc_t, w7_t, wf_t, we_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, c, d, e, f, g, h, a, b, we_t, SHA512C3e);
  wf_t = SHA384_EXPAND (wd_t, w8_t, w0_t, wf_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, b, c, d, e, f, g, h, a, wf_t, SHA512C3f);
  w0_t = SHA384_EXPAND (we_t, w9_t, w1_t, w0_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, a, b, c, d, e, f, g, h, w0_t, SHA512C40);
  w1_t = SHA384_EXPAND (wf_t, wa_t, w2_t, w1_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, h, a, b, c, d, e, f, g, w1_t, SHA512C41);
  w2_t = SHA384_EXPAND (w0_t, wb_t, w3_t, w2_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, g, h, a, b, c, d, e, f, w2_t, SHA512C42);
  w3_t = SHA384_EXPAND (w1_t, wc_t, w4_t, w3_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, f, g, h, a, b, c, d, e, w3_t, SHA512C43);
  w4_t = SHA384_EXPAND (w2_t, wd_t, w5_t, w4_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, e, f, g, h, a, b, c, d, w4_t, SHA512C44);
  w5_t = SHA384_EXPAND (w3_t, we_t, w6_t, w5_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, d, e, f, g, h, a, b, c, w5_t, SHA512C45);
  w6_t = SHA384_EXPAND (w4_t, wf_t, w7_t, w6_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, c, d, e, f, g, h, a, b, w6_t, SHA512C46);
  w7_t = SHA384_EXPAND (w5_t, w0_t, w8_t, w7_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, b, c, d, e, f, g, h, a, w7_t, SHA512C47);
  w8_t = SHA384_EXPAND (w6_t, w1_t, w9_t, w8_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, a, b, c, d, e, f, g, h, w8_t, SHA512C48);
  w9_t = SHA384_EXPAND (w7_t, w2_t, wa_t, w9_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, h, a, b, c, d, e, f, g, w9_t, SHA512C49);
  wa_t = SHA384_EXPAND (w8_t, w3_t, wb_t, wa_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, g, h, a, b, c, d, e, f, wa_t, SHA512C4a);
  wb_t = SHA384_EXPAND (w9_t, w4_t, wc_t, wb_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, f, g, h, a, b, c, d, e, wb_t, SHA512C4b);
  wc_t = SHA384_EXPAND (wa_t, w5_t, wd_t, wc_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, e, f, g, h, a, b, c, d, wc_t, SHA512C4c);
  wd_t = SHA384_EXPAND (wb_t, w6_t, we_t, wd_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, d, e, f, g, h, a, b, c, wd_t, SHA512C4d);
  we_t = SHA384_EXPAND (wc_t, w7_t, wf_t, we_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, c, d, e, f, g, h, a, b, we_t, SHA512C4e);
  wf_t = SHA384_EXPAND (wd_t, w8_t, w0_t, wf_t); SHA384_STEP (SHA384_F0o, SHA384_F1o, b, c, d, e, f, g, h, a, wf_t, SHA512C4f);

  /* rev
  digest[0] += a;
  digest[1] += b;
  digest[2] += c;
  digest[3] += d;
  digest[4] += e;
  digest[5] += f;
  digest[6] += g;
  digest[7] += h;
  */

  digest[0] = a;
  digest[1] = b;
  digest[2] = c;
  digest[3] = d;
  digest[4] = e;
  digest[5] = f;
  digest[6] = 0;
  digest[7] = 0;
}

KERNEL_FQ KERNEL_FA void m10820_m04 (KERN_ATTR_RULES ())
{
  /**
   * modifier
   */

  const u64 lid = get_local_id (0);

  /**
   * base
   */

  const u64 gid = get_global_id (0);

  if (gid >= GID_CNT) return;

  u32 pw_buf0[4];
  u32 pw_buf1[4];

  pw_buf0[0] = pws[gid].i[0];
  pw_buf0[1] = pws[gid].i[1];
  pw_buf0[2] = pws[gid].i[2];
  pw_buf0[3] = pws[gid].i[3];
  pw_buf1[0] = pws[gid].i[4];
  pw_buf1[1] = pws[gid].i[5];
  pw_buf1[2] = pws[gid].i[6];
  pw_buf1[3] = pws[gid].i[7];

  const u32 pw_len = pws[gid].pw_len & 63;

  /**
   * salt
   */

  u32 salt_buf0[4];
  u32 salt_buf1[4];
  u32 salt_buf2[4];
  u32 salt_buf3[4];

  salt_buf0[0] = salt_bufs[SALT_POS_HOST].salt_buf[ 0];
  salt_buf0[1] = salt_bufs[SALT_POS_HOST].salt_buf[ 1];
  salt_buf0[2] = salt_bufs[SALT_POS_HOST].salt_buf[ 2];
  salt_buf0[3] = salt_bufs[SALT_POS_HOST].salt_buf[ 3];
  salt_buf1[0] = salt_bufs[SALT_POS_HOST].salt_buf[ 4];
  salt_buf1[1] = salt_bufs[SALT_POS_HOST].salt_buf[ 5];
  salt_buf1[2] = salt_bufs[SALT_POS_HOST].salt_buf[ 6];
  salt_buf1[3] = salt_bufs[SALT_POS_HOST].salt_buf[ 7];
  salt_buf2[0] = salt_bufs[SALT_POS_HOST].salt_buf[ 8];
  salt_buf2[1] = salt_bufs[SALT_POS_HOST].salt_buf[ 9];
  salt_buf2[2] = salt_bufs[SALT_POS_HOST].salt_buf[10];
  salt_buf2[3] = salt_bufs[SALT_POS_HOST].salt_buf[11];
  salt_buf3[0] = salt_bufs[SALT_POS_HOST].salt_buf[12];
  salt_buf3[1] = salt_bufs[SALT_POS_HOST].salt_buf[13];
  salt_buf3[2] = salt_bufs[SALT_POS_HOST].salt_buf[14];
  salt_buf3[3] = salt_bufs[SALT_POS_HOST].salt_buf[15];

  const u32 salt_len = salt_bufs[SALT_POS_HOST].salt_len;

  /**
   * loop
   */

  for (u32 il_pos = 0; il_pos < IL_CNT; il_pos += VECT_SIZE)
  {
    u32x w0[4] = { 0 };
    u32x w1[4] = { 0 };
    u32x w2[4] = { 0 };
    u32x w3[4] = { 0 };

    const u32x out_len = apply_rules_vect_optimized (pw_buf0, pw_buf1, pw_len, rules_buf, il_pos, w0, w1);

    /**
     * prepend salt
     */

    const u32x out_salt_len = out_len + salt_len;

    switch_buffer_by_offset_le_VV (w0, w1, w2, w3, salt_len);

    w0[0] |= salt_buf0[0];
    w0[1] |= salt_buf0[1];
    w0[2] |= salt_buf0[2];
    w0[3] |= salt_buf0[3];
    w1[0] |= salt_buf1[0];
    w1[1] |= salt_buf1[1];
    w1[2] |= salt_buf1[2];
    w1[3] |= salt_buf1[3];
    w2[0] |= salt_buf2[0];
    w2[1] |= salt_buf2[1];
    w2[2] |= salt_buf2[2];
    w2[3] |= salt_buf2[3];
    w3[0] |= salt_buf3[0];
    w3[1] |= salt_buf3[1];
    w3[2] |= salt_buf3[2];
    w3[3] |= salt_buf3[3];

    append_0x80_4x4_VV (w0, w1, w2, w3, out_salt_len);

    /**
     * sha512
     */

    u32x w0_t[4];
    u32x w1_t[4];
    u32x w2_t[4];
    u32x w3_t[4];

    w0_t[0] = hc_swap32 (w0[0]);
    w0_t[1] = hc_swap32 (w0[1]);
    w0_t[2] = hc_swap32 (w0[2]);
    w0_t[3] = hc_swap32 (w0[3]);
    w1_t[0] = hc_swap32 (w1[0]);
    w1_t[1] = hc_swap32 (w1[1]);
    w1_t[2] = hc_swap32 (w1[2]);
    w1_t[3] = hc_swap32 (w1[3]);
    w2_t[0] = hc_swap32 (w2[0]);
    w2_t[1] = hc_swap32 (w2[1]);
    w2_t[2] = hc_swap32 (w2[2]);
    w2_t[3] = hc_swap32 (w2[3]);
    w3_t[0] = hc_swap32 (w3[0]);
    w3_t[1] = hc_swap32 (w3[1]);
    w3_t[2] = 0;
    w3_t[3] = out_salt_len * 8;

    u64x digest[8];

    digest[0] = SHA384M_A;
    digest[1] = SHA384M_B;
    digest[2] = SHA384M_C;
    digest[3] = SHA384M_D;
    digest[4] = SHA384M_E;
    digest[5] = SHA384M_F;
    digest[6] = SHA384M_G;
    digest[7] = SHA384M_H;

    sha384_transform_intern (w0_t, w1_t, w2_t, w3_t, digest);

    const u32x r0 = l32_from_64 (digest[3]);
    const u32x r1 = h32_from_64 (digest[3]);
    const u32x r2 = l32_from_64 (digest[2]);
    const u32x r3 = h32_from_64 (digest[2]);

    COMPARE_M_SIMD (r0, r1, r2, r3);
  }
}

KERNEL_FQ KERNEL_FA void m10820_m08 (KERN_ATTR_RULES ())
{
}

KERNEL_FQ KERNEL_FA void m10820_m16 (KERN_ATTR_RULES ())
{
}

KERNEL_FQ KERNEL_FA void m10820_s04 (KERN_ATTR_RULES ())
{
  /**
   * modifier
   */

  const u64 lid = get_local_id (0);

  /**
   * base
   */

  const u64 gid = get_global_id (0);

  if (gid >= GID_CNT) return;

  u32 pw_buf0[4];
  u32 pw_buf1[4];

  pw_buf0[0] = pws[gid].i[0];
  pw_buf0[1] = pws[gid].i[1];
  pw_buf0[2] = pws[gid].i[2];
  pw_buf0[3] = pws[gid].i[3];
  pw_buf1[0] = pws[gid].i[4];
  pw_buf1[1] = pws[gid].i[5];
  pw_buf1[2] = pws[gid].i[6];
  pw_buf1[3] = pws[gid].i[7];

  const u32 pw_len = pws[gid].pw_len & 63;

  /**
   * salt
   */

  u32 salt_buf0[4];
  u32 salt_buf1[4];
  u32 salt_buf2[4];
  u32 salt_buf3[4];

  salt_buf0[0] = salt_bufs[SALT_POS_HOST].salt_buf[ 0];
  salt_buf0[1] = salt_bufs[SALT_POS_HOST].salt_buf[ 1];
  salt_buf0[2] = salt_bufs[SALT_POS_HOST].salt_buf[ 2];
  salt_buf0[3] = salt_bufs[SALT_POS_HOST].salt_buf[ 3];
  salt_buf1[0] = salt_bufs[SALT_POS_HOST].salt_buf[ 4];
  salt_buf1[1] = salt_bufs[SALT_POS_HOST].salt_buf[ 5];
  salt_buf1[2] = salt_bufs[SALT_POS_HOST].salt_buf[ 6];
  salt_buf1[3] = salt_bufs[SALT_POS_HOST].salt_buf[ 7];
  salt_buf2[0] = salt_bufs[SALT_POS_HOST].salt_buf[ 8];
  salt_buf2[1] = salt_bufs[SALT_POS_HOST].salt_buf[ 9];
  salt_buf2[2] = salt_bufs[SALT_POS_HOST].salt_buf[10];
  salt_buf2[3] = salt_bufs[SALT_POS_HOST].salt_buf[11];
  salt_buf3[0] = salt_bufs[SALT_POS_HOST].salt_buf[12];
  salt_buf3[1] = salt_bufs[SALT_POS_HOST].salt_buf[13];
  salt_buf3[2] = salt_bufs[SALT_POS_HOST].salt_buf[14];
  salt_buf3[3] = salt_bufs[SALT_POS_HOST].salt_buf[15];

  const u32 salt_len = salt_bufs[SALT_POS_HOST].salt_len;

  /**
   * digest
   */

  const u32 search[4] =
  {
    digests_buf[DIGESTS_OFFSET_HOST].digest_buf[DGST_R0],
    digests_buf[DIGESTS_OFFSET_HOST].digest_buf[DGST_R1],
    digests_buf[DIGESTS_OFFSET_HOST].digest_buf[DGST_R2],
    digests_buf[DIGESTS_OFFSET_HOST].digest_buf[DGST_R3]
  };

  /**
   * loop
   */

  for (u32 il_pos = 0; il_pos < IL_CNT; il_pos += VECT_SIZE)
  {
    u32x w0[4] = { 0 };
    u32x w1[4] = { 0 };
    u32x w2[4] = { 0 };
    u32x w3[4] = { 0 };

    const u32x out_len = apply_rules_vect_optimized (pw_buf0, pw_buf1, pw_len, rules_buf, il_pos, w0, w1);

    /**
     * prepend salt
     */

    const u32x out_salt_len = out_len + salt_len;

    switch_buffer_by_offset_le_VV (w0, w1, w2, w3, salt_len);

    w0[0] |= salt_buf0[0];
    w0[1] |= salt_buf0[1];
    w0[2] |= salt_buf0[2];
    w0[3] |= salt_buf0[3];
    w1[0] |= salt_buf1[0];
    w1[1] |= salt_buf1[1];
    w1[2] |= salt_buf1[2];
    w1[3] |= salt_buf1[3];
    w2[0] |= salt_buf2[0];
    w2[1] |= salt_buf2[1];
    w2[2] |= salt_buf2[2];
    w2[3] |= salt_buf2[3];
    w3[0] |= salt_buf3[0];
    w3[1] |= salt_buf3[1];
    w3[2] |= salt_buf3[2];
    w3[3] |= salt_buf3[3];

    append_0x80_4x4_VV (w0, w1, w2, w3, out_salt_len);

    /**
     * sha384
     */

    u32x w0_t[4];
    u32x w1_t[4];
    u32x w2_t[4];
    u32x w3_t[4];

    w0_t[0] = hc_swap32 (w0[0]);
    w0_t[1] = hc_swap32 (w0[1]);
    w0_t[2] = hc_swap32 (w0[2]);
    w0_t[3] = hc_swap32 (w0[3]);
    w1_t[0] = hc_swap32 (w1[0]);
    w1_t[1] = hc_swap32 (w1[1]);
    w1_t[2] = hc_swap32 (w1[2]);
    w1_t[3] = hc_swap32 (w1[3]);
    w2_t[0] = hc_swap32 (w2[0]);
    w2_t[1] = hc_swap32 (w2[1]);
    w2_t[2] = hc_swap32 (w2[2]);
    w2_t[3] = hc_swap32 (w2[3]);
    w3_t[0] = hc_swap32 (w3[0]);
    w3_t[1] = hc_swap32 (w3[1]);
    w3_t[2] = 0;
    w3_t[3] = out_salt_len * 8;

    u64x digest[8];

    digest[0] = SHA384M_A;
    digest[1] = SHA384M_B;
    digest[2] = SHA384M_C;
    digest[3] = SHA384M_D;
    digest[4] = SHA384M_E;
    digest[5] = SHA384M_F;
    digest[6] = SHA384M_G;
    digest[7] = SHA384M_H;

    sha384_transform_intern (w0_t, w1_t, w2_t, w3_t, digest);

    const u32x r0 = l32_from_64 (digest[3]);
    const u32x r1 = h32_from_64 (digest[3]);
    const u32x r2 = l32_from_64 (digest[2]);
    const u32x r3 = h32_from_64 (digest[2]);

    COMPARE_S_SIMD (r0, r1, r2, r3);
  }
}

KERNEL_FQ KERNEL_FA void m10820_s08 (KERN_ATTR_RULES ())
{
}

KERNEL_FQ KERNEL_FA void m10820_s16 (KERN_ATTR_RULES ())
{
}
