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
#include M2S(INCLUDE_PATH/inc_simd.cl)
#include M2S(INCLUDE_PATH/inc_hash_md5.cl)
#endif

#if   VECT_SIZE == 1
#define uint_to_hex_lower8(i) make_u32x (l_bin2asc[(i)])
#elif VECT_SIZE == 2
#define uint_to_hex_lower8(i) make_u32x (l_bin2asc[(i).s0], l_bin2asc[(i).s1])
#elif VECT_SIZE == 4
#define uint_to_hex_lower8(i) make_u32x (l_bin2asc[(i).s0], l_bin2asc[(i).s1], l_bin2asc[(i).s2], l_bin2asc[(i).s3])
#elif VECT_SIZE == 8
#define uint_to_hex_lower8(i) make_u32x (l_bin2asc[(i).s0], l_bin2asc[(i).s1], l_bin2asc[(i).s2], l_bin2asc[(i).s3], l_bin2asc[(i).s4], l_bin2asc[(i).s5], l_bin2asc[(i).s6], l_bin2asc[(i).s7])
#elif VECT_SIZE == 16
#define uint_to_hex_lower8(i) make_u32x (l_bin2asc[(i).s0], l_bin2asc[(i).s1], l_bin2asc[(i).s2], l_bin2asc[(i).s3], l_bin2asc[(i).s4], l_bin2asc[(i).s5], l_bin2asc[(i).s6], l_bin2asc[(i).s7], l_bin2asc[(i).s8], l_bin2asc[(i).s9], l_bin2asc[(i).sa], l_bin2asc[(i).sb], l_bin2asc[(i).sc], l_bin2asc[(i).sd], l_bin2asc[(i).se], l_bin2asc[(i).sf])
#endif

typedef struct md5_triple_salt
{
  u32 salt1_buf[64];
  int salt1_len;

  u32 salt2_buf[64];
  int salt2_len;

  u32 salt3_buf[64];
  int salt3_len;

} md5_triple_salt_t;

KERNEL_FQ KERNEL_FA void m32300_mxx (KERN_ATTR_VECTOR_ESALT (md5_triple_salt_t))
{
  /**
   * modifier
   */

  const u64 gid = get_global_id (0);
  const u64 lid = get_local_id (0);
  const u64 lsz = get_local_size (0);

  /*
  * bin2asc uppercase table
  */

  LOCAL_VK u32 l_bin2asc[256];

  for (u32 i = lid; i < 256; i += lsz)
  {
    const u32 i0 = (i >> 0) & 15;
    const u32 i1 = (i >> 4) & 15;

    l_bin2asc[i] = ((i0 < 10) ? '0' + i0 : 'a' - 10 + i0) << 8
                 | ((i1 < 10) ? '0' + i1 : 'a' - 10 + i1) << 0;
  }

  SYNC_THREADS ();

  if (gid >= GID_CNT) return;

  /**
   * base
   */

  const u32 pw_len = pws[gid].pw_len;

  u32x w[64] = { 0 };

  for (u32 i = 0, idx = 0; i < pw_len; i += 4, idx += 1)
  {
    w[idx] = pws[gid].i[idx];
  }

  const u32 salt1_len = esalt_bufs[DIGESTS_OFFSET_HOST].salt1_len;

  u32x salt1_buf[64] = { 0 };

  for (u32 i = 0, idx = 0; i < salt1_len; i += 4, idx += 1)
  {
    salt1_buf[idx] = esalt_bufs[DIGESTS_OFFSET_HOST].salt1_buf[idx];
  }

  const u32 salt2_len = esalt_bufs[DIGESTS_OFFSET_HOST].salt2_len;

  u32x salt2_buf[64] = { 0 };

  for (u32 i = 0, idx = 0; i < salt2_len; i += 4, idx += 1)
  {
    salt2_buf[idx] = esalt_bufs[DIGESTS_OFFSET_HOST].salt2_buf[idx];
  }

  const u32 salt3_len = esalt_bufs[DIGESTS_OFFSET_HOST].salt3_len;

  u32x salt3_buf[64] = { 0 };

  for (u32 i = 0, idx = 0; i < salt3_len; i += 4, idx += 1)
  {
    salt3_buf[idx] = esalt_bufs[DIGESTS_OFFSET_HOST].salt3_buf[idx];
  }

  /**
   * loop
   */

  u32x w0l = w[0];

  for (u32 il_pos = 0; il_pos < IL_CNT; il_pos += VECT_SIZE)
  {
    const u32x w0r = words_buf_r[il_pos / VECT_SIZE];

    const u32x w0 = w0l | w0r;

    w[0] = w0;

    md5_ctx_vector_t ctx0;

    md5_init_vector (&ctx0);

    md5_update_vector (&ctx0, w, pw_len);

    md5_final_vector (&ctx0);

    u32x a = ctx0.h[0];
    u32x b = ctx0.h[1];
    u32x c = ctx0.h[2];
    u32x d = ctx0.h[3];

    md5_ctx_vector_t ctx;

    md5_init_vector (&ctx);

    ctx.w0[0] = uint_to_hex_lower8 ((a >>  0) & 255) <<  0
              | uint_to_hex_lower8 ((a >>  8) & 255) << 16;
    ctx.w0[1] = uint_to_hex_lower8 ((a >> 16) & 255) <<  0
              | uint_to_hex_lower8 ((a >> 24) & 255) << 16;
    ctx.w0[2] = uint_to_hex_lower8 ((b >>  0) & 255) <<  0
              | uint_to_hex_lower8 ((b >>  8) & 255) << 16;
    ctx.w0[3] = uint_to_hex_lower8 ((b >> 16) & 255) <<  0
              | uint_to_hex_lower8 ((b >> 24) & 255) << 16;
    ctx.w1[0] = uint_to_hex_lower8 ((c >>  0) & 255) <<  0
              | uint_to_hex_lower8 ((c >>  8) & 255) << 16;
    ctx.w1[1] = uint_to_hex_lower8 ((c >> 16) & 255) <<  0
              | uint_to_hex_lower8 ((c >> 24) & 255) << 16;
    ctx.w1[2] = uint_to_hex_lower8 ((d >>  0) & 255) <<  0
              | uint_to_hex_lower8 ((d >>  8) & 255) << 16;
    ctx.w1[3] = uint_to_hex_lower8 ((d >> 16) & 255) <<  0
              | uint_to_hex_lower8 ((d >> 24) & 255) << 16;

    ctx.len = 32;

    md5_update_vector (&ctx, salt1_buf, salt1_len);

    md5_final_vector (&ctx);

    a = ctx.h[0];
    b = ctx.h[1];
    c = ctx.h[2];
    d = ctx.h[3];

    md5_init_vector (&ctx);

    md5_update_vector (&ctx, salt2_buf, salt2_len);

    u32x ww0[4];
    u32x ww1[4];
    u32x ww2[4];
    u32x ww3[4];

    ww0[0] = uint_to_hex_lower8 ((a >>  0) & 255) <<  0
           | uint_to_hex_lower8 ((a >>  8) & 255) << 16;
    ww0[1] = uint_to_hex_lower8 ((a >> 16) & 255) <<  0
           | uint_to_hex_lower8 ((a >> 24) & 255) << 16;
    ww0[2] = uint_to_hex_lower8 ((b >>  0) & 255) <<  0
           | uint_to_hex_lower8 ((b >>  8) & 255) << 16;
    ww0[3] = uint_to_hex_lower8 ((b >> 16) & 255) <<  0
           | uint_to_hex_lower8 ((b >> 24) & 255) << 16;
    ww1[0] = uint_to_hex_lower8 ((c >>  0) & 255) <<  0
           | uint_to_hex_lower8 ((c >>  8) & 255) << 16;
    ww1[1] = uint_to_hex_lower8 ((c >> 16) & 255) <<  0
           | uint_to_hex_lower8 ((c >> 24) & 255) << 16;
    ww1[2] = uint_to_hex_lower8 ((d >>  0) & 255) <<  0
           | uint_to_hex_lower8 ((d >>  8) & 255) << 16;
    ww1[3] = uint_to_hex_lower8 ((d >> 16) & 255) <<  0
           | uint_to_hex_lower8 ((d >> 24) & 255) << 16;

    ww2[0] = 0;
    ww2[1] = 0;
    ww2[2] = 0;
    ww2[3] = 0;
    ww3[0] = 0;
    ww3[1] = 0;
    ww3[2] = 0;
    ww3[3] = 0;

    md5_update_vector_64 (&ctx, ww0, ww1, ww2, ww3, 32);

    md5_update_vector (&ctx, salt3_buf, salt3_len);

    md5_final_vector (&ctx);

    const u32x r0 = ctx.h[DGST_R0];
    const u32x r1 = ctx.h[DGST_R1];
    const u32x r2 = ctx.h[DGST_R2];
    const u32x r3 = ctx.h[DGST_R3];

    COMPARE_M_SIMD (r0, r1, r2, r3);
  }
}

KERNEL_FQ KERNEL_FA void m32300_sxx (KERN_ATTR_VECTOR_ESALT (md5_triple_salt_t))
{
  /**
   * modifier
   */

  const u64 gid = get_global_id (0);
  const u64 lid = get_local_id (0);
  const u64 lsz = get_local_size (0);

  /*
  * bin2asc uppercase table
  */

  LOCAL_VK u32 l_bin2asc[256];

  for (u32 i = lid; i < 256; i += lsz)
  {
    const u32 i0 = (i >> 0) & 15;
    const u32 i1 = (i >> 4) & 15;

    l_bin2asc[i] = ((i0 < 10) ? '0' + i0 : 'a' - 10 + i0) << 8
                 | ((i1 < 10) ? '0' + i1 : 'a' - 10 + i1) << 0;
  }

  SYNC_THREADS ();

  if (gid >= GID_CNT) return;

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
   * base
   */

  const u32 pw_len = pws[gid].pw_len;

  u32x w[64] = { 0 };

  for (u32 i = 0, idx = 0; i < pw_len; i += 4, idx += 1)
  {
    w[idx] = pws[gid].i[idx];
  }

  const u32 salt1_len = esalt_bufs[DIGESTS_OFFSET_HOST].salt1_len;

  u32x salt1_buf[64] = { 0 };

  for (u32 i = 0, idx = 0; i < salt1_len; i += 4, idx += 1)
  {
    salt1_buf[idx] = esalt_bufs[DIGESTS_OFFSET_HOST].salt1_buf[idx];
  }

  const u32 salt2_len = esalt_bufs[DIGESTS_OFFSET_HOST].salt2_len;

  u32x salt2_buf[64] = { 0 };

  for (u32 i = 0, idx = 0; i < salt2_len; i += 4, idx += 1)
  {
    salt2_buf[idx] = esalt_bufs[DIGESTS_OFFSET_HOST].salt2_buf[idx];
  }

  const u32 salt3_len = esalt_bufs[DIGESTS_OFFSET_HOST].salt3_len;

  u32x salt3_buf[64] = { 0 };

  for (u32 i = 0, idx = 0; i < salt3_len; i += 4, idx += 1)
  {
    salt3_buf[idx] = esalt_bufs[DIGESTS_OFFSET_HOST].salt3_buf[idx];
  }

  /**
   * loop
   */

  u32x w0l = w[0];

  for (u32 il_pos = 0; il_pos < IL_CNT; il_pos += VECT_SIZE)
  {
    const u32x w0r = words_buf_r[il_pos / VECT_SIZE];

    const u32x w0 = w0l | w0r;

    w[0] = w0;

    md5_ctx_vector_t ctx0;

    md5_init_vector (&ctx0);

    md5_update_vector (&ctx0, w, pw_len);

    md5_final_vector (&ctx0);

    u32x a = ctx0.h[0];
    u32x b = ctx0.h[1];
    u32x c = ctx0.h[2];
    u32x d = ctx0.h[3];

    md5_ctx_vector_t ctx;

    md5_init_vector (&ctx);

    ctx.w0[0] = uint_to_hex_lower8 ((a >>  0) & 255) <<  0
              | uint_to_hex_lower8 ((a >>  8) & 255) << 16;
    ctx.w0[1] = uint_to_hex_lower8 ((a >> 16) & 255) <<  0
              | uint_to_hex_lower8 ((a >> 24) & 255) << 16;
    ctx.w0[2] = uint_to_hex_lower8 ((b >>  0) & 255) <<  0
              | uint_to_hex_lower8 ((b >>  8) & 255) << 16;
    ctx.w0[3] = uint_to_hex_lower8 ((b >> 16) & 255) <<  0
              | uint_to_hex_lower8 ((b >> 24) & 255) << 16;
    ctx.w1[0] = uint_to_hex_lower8 ((c >>  0) & 255) <<  0
              | uint_to_hex_lower8 ((c >>  8) & 255) << 16;
    ctx.w1[1] = uint_to_hex_lower8 ((c >> 16) & 255) <<  0
              | uint_to_hex_lower8 ((c >> 24) & 255) << 16;
    ctx.w1[2] = uint_to_hex_lower8 ((d >>  0) & 255) <<  0
              | uint_to_hex_lower8 ((d >>  8) & 255) << 16;
    ctx.w1[3] = uint_to_hex_lower8 ((d >> 16) & 255) <<  0
              | uint_to_hex_lower8 ((d >> 24) & 255) << 16;

    ctx.len = 32;

    md5_update_vector (&ctx, salt1_buf, salt1_len);

    md5_final_vector (&ctx);

    a = ctx.h[0];
    b = ctx.h[1];
    c = ctx.h[2];
    d = ctx.h[3];

    md5_init_vector (&ctx);

    md5_update_vector (&ctx, salt2_buf, salt2_len);

    u32x ww0[4];
    u32x ww1[4];
    u32x ww2[4];
    u32x ww3[4];

    ww0[0] = uint_to_hex_lower8 ((a >>  0) & 255) <<  0
           | uint_to_hex_lower8 ((a >>  8) & 255) << 16;
    ww0[1] = uint_to_hex_lower8 ((a >> 16) & 255) <<  0
           | uint_to_hex_lower8 ((a >> 24) & 255) << 16;
    ww0[2] = uint_to_hex_lower8 ((b >>  0) & 255) <<  0
           | uint_to_hex_lower8 ((b >>  8) & 255) << 16;
    ww0[3] = uint_to_hex_lower8 ((b >> 16) & 255) <<  0
           | uint_to_hex_lower8 ((b >> 24) & 255) << 16;
    ww1[0] = uint_to_hex_lower8 ((c >>  0) & 255) <<  0
           | uint_to_hex_lower8 ((c >>  8) & 255) << 16;
    ww1[1] = uint_to_hex_lower8 ((c >> 16) & 255) <<  0
           | uint_to_hex_lower8 ((c >> 24) & 255) << 16;
    ww1[2] = uint_to_hex_lower8 ((d >>  0) & 255) <<  0
           | uint_to_hex_lower8 ((d >>  8) & 255) << 16;
    ww1[3] = uint_to_hex_lower8 ((d >> 16) & 255) <<  0
           | uint_to_hex_lower8 ((d >> 24) & 255) << 16;

    ww2[0] = 0;
    ww2[1] = 0;
    ww2[2] = 0;
    ww2[3] = 0;
    ww3[0] = 0;
    ww3[1] = 0;
    ww3[2] = 0;
    ww3[3] = 0;

    md5_update_vector_64 (&ctx, ww0, ww1, ww2, ww3, 32);

    md5_update_vector (&ctx, salt3_buf, salt3_len);

    md5_final_vector (&ctx);

    const u32x r0 = ctx.h[DGST_R0];
    const u32x r1 = ctx.h[DGST_R1];
    const u32x r2 = ctx.h[DGST_R2];
    const u32x r3 = ctx.h[DGST_R3];

    COMPARE_S_SIMD (r0, r1, r2, r3);
  }
}
