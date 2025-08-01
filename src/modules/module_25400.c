/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

// https://www.adobe.com/content/dam/acom/en/devnet/pdf/pdfs/pdf_reference_archives/PDFReference.pdf

#include "common.h"
#include "types.h"
#include "modules.h"
#include "bitops.h"
#include "convert.h"
#include "shared.h"
#include "emu_inc_hash_md5.h"

static const u32   ATTACK_EXEC    = ATTACK_EXEC_OUTSIDE_KERNEL;
static const u32   DGST_POS0      = 0;
static const u32   DGST_POS1      = 1;
static const u32   DGST_POS2      = 2;
static const u32   DGST_POS3      = 3;
static const u32   DGST_SIZE      = DGST_SIZE_4_4;
static const u32   HASH_CATEGORY  = HASH_CATEGORY_DOCUMENTS;
static const char *HASH_NAME      = "PDF 1.4 - 1.6 (Acrobat 5 - 8) - user and owner pass";
static const u64   KERN_TYPE      = 25400;
static const u32   OPTI_TYPE      = OPTI_TYPE_ZERO_BYTE
                                  | OPTI_TYPE_NOT_ITERATED;
static const u64   OPTS_TYPE      = OPTS_TYPE_STOCK_MODULE
                                  | OPTS_TYPE_PT_GENERATE_LE
                                  | OPTS_TYPE_COPY_TMPS
                                  | OPTS_TYPE_PT_ALWAYS_ASCII
                                  | OPTS_TYPE_AUTODETECT_DISABLE;
static const u32   SALT_TYPE      = SALT_TYPE_EMBEDDED;
static const char *ST_PASS        = "hashcat";
static const char *ST_HASH        = "$pdf$2*3*128*-3904*1*16*631ed33746e50fba5caf56bcc39e09c6*32*5f9d0e4f0b39835dace0d306c40cd6b700000000000000000000000000000000*32*842103b0a0dc886db9223b94afe2d7cd63389079b61986a4fcf70095ad630c24";

u32         module_attack_exec    (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return ATTACK_EXEC;     }
u32         module_dgst_pos0      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS0;       }
u32         module_dgst_pos1      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS1;       }
u32         module_dgst_pos2      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS2;       }
u32         module_dgst_pos3      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS3;       }
u32         module_dgst_size      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_SIZE;       }
u32         module_hash_category  (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return HASH_CATEGORY;   }
const char *module_hash_name      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return HASH_NAME;       }
u64         module_kern_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return KERN_TYPE;       }
u32         module_opti_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return OPTI_TYPE;       }
u64         module_opts_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return OPTS_TYPE;       }
u32         module_salt_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return SALT_TYPE;       }
const char *module_st_hash        (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return ST_HASH;         }
const char *module_st_pass        (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return ST_PASS;         }

typedef struct pdf
{
  int V;
  int R;
  int P;

  int enc_md;

  u32 id_buf[8];
  u32 u_buf[32];
  u32 o_buf[32];
  u32 u_pass_buf[8];

  int id_len;
  int o_len;
  int u_len;
  int u_pass_len;

  u32 rc4key[2];
  u32 rc4data[2];

  int P_minus;

} pdf_t;

typedef struct pdf14_tmp
{
  u32 digest[4];
  u32 out[8];

} pdf14_tmp_t;

static const char *SIGNATURE_PDF = "$pdf$";

u32 module_kernel_loops_min (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u32 kernel_loops_min = 70;

  return kernel_loops_min;
}

u32 module_kernel_loops_max (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u32 kernel_loops_max = 70;

  return kernel_loops_max;
}

static void md5_complete_no_limit (u32 digest[4], const u32 *plain, const u32 plain_len)
{
  // plain = u32 tmp_md5_buf[64] so this is compatible

  md5_ctx_t md5_ctx;

  md5_init (&md5_ctx);
  md5_update (&md5_ctx, plain, plain_len);
  md5_final (&md5_ctx);

  digest[0] = md5_ctx.h[0];
  digest[1] = md5_ctx.h[1];
  digest[2] = md5_ctx.h[2];
  digest[3] = md5_ctx.h[3];
}

bool module_unstable_warning (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra, MAYBE_UNUSED const hc_device_param_t *device_param)
{
  if ((device_param->opencl_platform_vendor_id == VENDOR_ID_APPLE) && (device_param->opencl_device_type & CL_DEVICE_TYPE_GPU))
  {
    if (device_param->is_metal == true)
    {
      if (strncmp (device_param->device_name, "Intel", 5) == 0)
      {
        // Intel Iris Graphics, Metal Version 244.303: self-test failed
        return true;
      }
    }
  }

  return false;
}

char *module_jit_build_options (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra, MAYBE_UNUSED const hashes_t *hashes, MAYBE_UNUSED const hc_device_param_t *device_param)
{
  char *jit_build_options = NULL;

  // We must override whatever tuningdb entry or -T value was set by the user with this
  // That's because of RC code in inc_cipher_rc4.cl has this for GPU (different on CPU):
  // #define KEY32(t,k) (((k) * 32) + ((t) & 31) + (((t) / 32) * 2048))
  // #define KEY8(t,k) (((k) & 3) + (((k) / 4) * 128) + (((t) & 31) * 4) + (((t) / 32) * 8192))

  u32 native_threads = (device_param->opencl_device_type & CL_DEVICE_TYPE_CPU) ? 1 : 32;

  hc_asprintf (&jit_build_options, "-D FIXED_LOCAL_SIZE=%u", native_threads);

  return jit_build_options;
}

u64 module_esalt_size (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u64 esalt_size = (const u64) sizeof (pdf_t);

  return esalt_size;
}

u64 module_tmp_size (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u64 tmp_size = (const u64) sizeof (pdf14_tmp_t);

  return tmp_size;
}

u32 module_pw_max (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u32 pw_max = 32; // "truncate the password string to exactly 32 bytes." see "Algorithm 3.2 computing an encryption key"
  return pw_max;
}

int module_hash_decode (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED void *digest_buf, MAYBE_UNUSED salt_t *salt, MAYBE_UNUSED void *esalt_buf, MAYBE_UNUSED void *hook_salt_buf, MAYBE_UNUSED hashinfo_t *hash_info, const char *line_buf, MAYBE_UNUSED const int line_len)
{
  const char *input_buf = line_buf;
  int   input_len = line_len;

  // based on m22000 module_hash_decode() we detect both the hashformat with and without user-password
  u32 *digest = (u32 *) digest_buf;

  pdf_t *pdf = (pdf_t *) esalt_buf;

  hc_token_t token;

  memset (&token, 0, sizeof (hc_token_t));

  token.token_cnt  = 12;

  token.signatures_cnt    = 1;
  token.signatures_buf[0] = SIGNATURE_PDF;

  token.len[0]     = 5;
  token.attr[0]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_SIGNATURE;

  token.sep[1]     = '*';
  token.len[1]     = 1;
  token.attr[1]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[2]     = '*';
  token.len[2]     = 1;
  token.attr[2]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[3]     = '*';
  token.len[3]     = 3;
  token.attr[3]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[4]     = '*';
  token.len_min[4] = 1;
  token.len_max[4] = 11;
  token.attr[4]    = TOKEN_ATTR_VERIFY_LENGTH;

  token.sep[5]     = '*';
  token.len[5]     = 1;
  token.attr[5]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[6]     = '*';
  token.len[6]     = 2;
  token.attr[6]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[7]     = '*';
  token.len_min[7] = 32;
  token.len_max[7] = 64;
  token.attr[7]    = TOKEN_ATTR_VERIFY_LENGTH
                   | TOKEN_ATTR_VERIFY_HEX;

  token.sep[8]     = '*';
  token.len[8]     = 2;
  token.attr[8]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[9]     = '*';
  token.len[9]     = 64;
  token.attr[9]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_HEX;

  token.sep[10]    = '*';
  token.len[10]    = 2;
  token.attr[10]   = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[11]    = '*';
  token.len[11]    = 64;
  token.attr[11]   = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_HEX;

  int rc_tokenizer = input_tokenizer ((const u8 *) line_buf, line_len, &token); // was a const, now no longer, as we need it again for the new hashformat

  //check if hashformat without user-password is detected
  if (rc_tokenizer == PARSER_OK)
  {
    char tmp_buf[1024];
    int  tmp_len;

    tmp_len = snprintf (tmp_buf, sizeof (tmp_buf), "%s*", line_buf); // simply add an extra asterisk to denote a empty user-password

    input_buf = tmp_buf;
    input_len = tmp_len;
  }

  token.token_cnt   = 13;

  token.signatures_cnt    = 1;
  token.signatures_buf[0] = SIGNATURE_PDF;

  token.len[0]      = 5;
  token.attr[0]     = TOKEN_ATTR_FIXED_LENGTH
                    | TOKEN_ATTR_VERIFY_SIGNATURE;

  token.sep[1]      = '*';
  token.len[1]      = 1;
  token.attr[1]     = TOKEN_ATTR_FIXED_LENGTH
                    | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[2]      = '*';
  token.len[2]      = 1;
  token.attr[2]     = TOKEN_ATTR_FIXED_LENGTH
                    | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[3]      = '*';
  token.len[3]      = 3;
  token.attr[3]     = TOKEN_ATTR_FIXED_LENGTH
                    | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[4]      = '*';
  token.len_min[4]  = 1;
  token.len_max[4]  = 6;
  token.attr[4]     = TOKEN_ATTR_VERIFY_LENGTH;

  token.sep[5]      = '*';
  token.len[5]      = 1;
  token.attr[5]     = TOKEN_ATTR_FIXED_LENGTH
                    | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[6]      = '*';
  token.len[6]      = 2;
  token.attr[6]     = TOKEN_ATTR_FIXED_LENGTH
                    | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[7]      = '*';
  token.len_min[7]  = 32;
  token.len_max[7]  = 64;
  token.attr[7]     = TOKEN_ATTR_VERIFY_LENGTH
                    | TOKEN_ATTR_VERIFY_HEX;

  token.sep[8]      = '*';
  token.len[8]      = 2;
  token.attr[8]     = TOKEN_ATTR_FIXED_LENGTH
                    | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[9]      = '*';
  token.len[9]      = 64;
  token.attr[9]     = TOKEN_ATTR_FIXED_LENGTH
                    | TOKEN_ATTR_VERIFY_HEX;

  token.sep[10]     = '*';
  token.len[10]     = 2;
  token.attr[10]    = TOKEN_ATTR_FIXED_LENGTH
                    | TOKEN_ATTR_VERIFY_DIGIT;

  token.sep[11]     = '*';
  token.len[11]     = 64;
  token.attr[11]    = TOKEN_ATTR_FIXED_LENGTH
                    | TOKEN_ATTR_VERIFY_HEX;

  token.sep[12]     = '*';
  token.len_min[12] = 0;
  token.len_max[12] = 32; // "truncate the password string to exactly 32 bytes." see "Algorithm 3.2 computing an encryption key"
  token.attr[12]    = TOKEN_ATTR_VERIFY_LENGTH;

  rc_tokenizer = input_tokenizer ((const u8 *) input_buf, input_len, &token);

  // detect hashformat including the user-password
  if (rc_tokenizer != PARSER_OK) return (rc_tokenizer);

  const u8 *V_pos      = token.buf[1];
  const u8 *R_pos      = token.buf[2];
  const u8 *bits_pos   = token.buf[3];
  const u8 *P_pos      = token.buf[4];
  const u8 *enc_md_pos = token.buf[5];
  const u8 *id_len_pos = token.buf[6];
  const u8 *id_buf_pos = token.buf[7];
  const u8 *u_len_pos  = token.buf[8];
  const u8 *u_buf_pos  = token.buf[9];  // user hash
  const u8 *o_len_pos  = token.buf[10];
  const u8 *o_buf_pos  = token.buf[11]; // owner hash
  const u8 *u_pass_buf_pos  = token.buf[12]; // user password (optional)
  // we don't use the user-password in the attack now (as we don't need it),
  //  however we could use it in the comparison of the decrypted o-value,
  //  yet it may make this attack a bit more fragile, as now we just check for ASCII

  // validate data

  pdf->P_minus = 0;

  if (P_pos[0] == 0x2d) pdf->P_minus = 1;

  const int V = strtol ((const char *) V_pos, NULL, 10);
  const int R = strtol ((const char *) R_pos, NULL, 10);
  const int P = strtol ((const char *) P_pos, NULL, 10);

  int vr_ok = 0;

  if ((V == 2) && (R == 3)) vr_ok = 1;
  if ((V == 4) && (R == 4)) vr_ok = 1;

  if (vr_ok == 0) return (PARSER_SALT_VALUE);

  const int id_len = strtol ((const char *) id_len_pos, NULL, 10);
  const int u_len  = strtol ((const char *) u_len_pos,  NULL, 10);
  const int o_len  = strtol ((const char *) o_len_pos,  NULL, 10);

  if ((id_len != 16) && (id_len != 32)) return (PARSER_SALT_VALUE);

  if (u_len != 32) return (PARSER_SALT_VALUE);
  if (o_len != 32) return (PARSER_SALT_VALUE);

  const int bits = strtol ((const char *) bits_pos, NULL, 10);

  if (bits != 128) return (PARSER_SALT_VALUE);

  int enc_md = 1;

  if (R >= 4)
  {
    enc_md = strtol ((const char *) enc_md_pos, NULL, 10);
  }

  // copy data to esalt
  pdf->V = V;
  pdf->R = R;
  pdf->P = P;

  memcpy ( pdf->u_pass_buf, u_pass_buf_pos, 32);
  pdf->u_pass_len = strlen ((char *) pdf->u_pass_buf);

  pdf->enc_md = enc_md;

  pdf->id_buf[0] = hex_to_u32 (id_buf_pos +  0);
  pdf->id_buf[1] = hex_to_u32 (id_buf_pos +  8);
  pdf->id_buf[2] = hex_to_u32 (id_buf_pos + 16);
  pdf->id_buf[3] = hex_to_u32 (id_buf_pos + 24);

  if (id_len == 32)
  {
    pdf->id_buf[4] = hex_to_u32 (id_buf_pos + 32);
    pdf->id_buf[5] = hex_to_u32 (id_buf_pos + 40);
    pdf->id_buf[6] = hex_to_u32 (id_buf_pos + 48);
    pdf->id_buf[7] = hex_to_u32 (id_buf_pos + 56);
  }

  pdf->id_len = id_len;

  pdf->u_buf[0]  = hex_to_u32 (u_buf_pos +  0);
  pdf->u_buf[1]  = hex_to_u32 (u_buf_pos +  8);
  pdf->u_buf[2]  = hex_to_u32 (u_buf_pos + 16);
  pdf->u_buf[3]  = hex_to_u32 (u_buf_pos + 24);
  pdf->u_buf[4]  = hex_to_u32 (u_buf_pos + 32);
  pdf->u_buf[5]  = hex_to_u32 (u_buf_pos + 40);
  pdf->u_buf[6]  = hex_to_u32 (u_buf_pos + 48);
  pdf->u_buf[7]  = hex_to_u32 (u_buf_pos + 56);
  pdf->u_len     = u_len;

  pdf->o_buf[0]  = hex_to_u32 (o_buf_pos +  0);
  pdf->o_buf[1]  = hex_to_u32 (o_buf_pos +  8);
  pdf->o_buf[2]  = hex_to_u32 (o_buf_pos + 16);
  pdf->o_buf[3]  = hex_to_u32 (o_buf_pos + 24);
  pdf->o_buf[4]  = hex_to_u32 (o_buf_pos + 32);
  pdf->o_buf[5]  = hex_to_u32 (o_buf_pos + 40);
  pdf->o_buf[6]  = hex_to_u32 (o_buf_pos + 48);
  pdf->o_buf[7]  = hex_to_u32 (o_buf_pos + 56);
  pdf->o_len     = o_len;

  // precompute rc4 data for later use
  u32 padding[8] =
  {
    0x5e4ebf28,
    0x418a754e,
    0x564e0064,
    0x0801faff,
    0xb6002e2e,
    0x803e68d0,
    0xfea90c2f,
    0x7a695364
  };

  // md5
  u32 salt_pc_block[32] = { 0 };

  u8 *salt_pc_ptr = (u8 *) salt_pc_block;

  memcpy (salt_pc_ptr, padding, 32);
  memcpy (salt_pc_ptr + 32, pdf->id_buf, pdf->id_len);

  u32 salt_pc_digest[4] = { 0 };

  md5_complete_no_limit (salt_pc_digest, salt_pc_block, 32 + pdf->id_len);

  pdf->rc4data[0] = salt_pc_digest[0];
  pdf->rc4data[1] = salt_pc_digest[1];

  // we use ID for salt, maybe needs to change, we will see...
  salt->salt_buf[0] = pdf->id_buf[0];
  salt->salt_buf[1] = pdf->id_buf[1];
  salt->salt_buf[2] = pdf->id_buf[2];
  salt->salt_buf[3] = pdf->id_buf[3];
  salt->salt_buf[4] = pdf->o_buf[0]; // switched u_buf with o_buf vs m10500
  salt->salt_buf[5] = pdf->o_buf[1];
  salt->salt_buf[6] = pdf->u_buf[0];
  salt->salt_buf[7] = pdf->u_buf[1];
  salt->salt_len    = pdf->id_len + 16;

  salt->salt_iter   = (50 + 20);

  digest[0] = pdf->o_buf[0]; // o_buf instead of u_buf vs m10500
  digest[1] = pdf->o_buf[1];
  digest[2] = 0;
  digest[3] = 0;

  return (PARSER_OK);
}

int module_build_plain_postprocess (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const hashes_t *hashes, MAYBE_UNUSED const void *tmps, const u32 *src_buf, MAYBE_UNUSED const size_t src_sz, MAYBE_UNUSED const int src_len, u32 *dst_buf, MAYBE_UNUSED const size_t dst_sz)
{
  const u32 padding[8] =
  {
    0x5e4ebf28,
    0x418a754e,
    0x564e0064,
    0x0801faff,
    0xb6002e2e,
    0x803e68d0,
    0xfea90c2f,
    0x7a695364
  };

  const pdf14_tmp_t *pdf_tmp = (const pdf14_tmp_t *) tmps;
  pdf_t *pdf = (pdf_t *) hashes->esalts_buf;

  // if the password in tmp->out is equal to the padding, then we recovered just the owner-password

  if (pdf_tmp->out[0] == padding[0] &&
      pdf_tmp->out[1] == padding[1] &&
      pdf_tmp->out[2] == padding[2] &&
      pdf_tmp->out[3] == padding[3] &&
      pdf_tmp->out[4] == padding[4] &&
      pdf_tmp->out[5] == padding[5] &&
      pdf_tmp->out[6] == padding[6] &&
      pdf_tmp->out[7] == padding[7])
  {
    return snprintf ((char *) dst_buf, dst_sz, "%s    (user password not set)", (const char *) src_buf);
  }

  // cast out buffer to byte such that we can do a byte per byte comparison
  const u32 *u32OutBufPtr = pdf_tmp->out;
  u8 *u8OutBufPtr;
  u8OutBufPtr = (u8*) u32OutBufPtr;

  // cast padding buffer to byte such that we can do a byte per byte comparison
  const u32 *u32OutPadPtr = padding;
  const u8 *u8OutPadPtr;
  u8OutPadPtr = (u8*) u32OutPadPtr;

  bool remove_padding = false;
  int i_padding = 0;

  for (int i = 0; i < 16; i++)
  {
    if (u8OutBufPtr[i] == u8OutPadPtr[i_padding] || remove_padding)
    {
      u8OutBufPtr[i] = 0x0;
      remove_padding = true;
    }
  }

  // if the password in tmp->out is equal to the password tried, then we recovered just the owner-password or just the user-password
  //   we check whether we already have a user-password in the hash
  //   TODO would be better to actually also verify the u-value whether we've retrieved the correct user-password,
  //     however, we'd need to include a lot of code/complexity here to do so (or call into 10500 kernel).
  //     this seems relevant: run_kernel (hashcat_ctx, device_param, KERN_RUN_3, 0, 1, false, 0)

  if (pdf_tmp->out[0] == padding[0] &&
      pdf_tmp->out[1] == padding[1] &&
      pdf_tmp->out[2] == padding[2] &&
      pdf_tmp->out[3] == padding[3] &&
      pdf_tmp->out[4] == padding[4] &&
      pdf_tmp->out[5] == padding[5] &&
      pdf_tmp->out[6] == padding[6] &&
      pdf_tmp->out[7] == padding[7])
  {
    if (pdf->u_pass_len == 0)
    {
      // we seem to only have recovered the user-password as we don't have one yet
      return snprintf ((char *) dst_buf, dst_sz, "(user password=%s)", (const char *) src_buf);
    }
  }
  // we recovered both the user-password and the owner-password
  return snprintf ((char *) dst_buf, dst_sz, "%s    (user password=%s)", (const char *) src_buf, (const char *) pdf_tmp->out);
}

// TODO how to add the recovered user-password to the hash?
// module_hash_encode() is called before module_build_plain_postprocess() is
//  module_hash_encode() doesn't know the recovered password src_buf or the decrypted o-value pdf_tmp->out
//    it seems a bit excessive to add these both to module_hash_encode()'s parameters
//  module_build_plain_postprocess() cannot alter the hash nor hash access to the pdf/esalt object
int module_hash_encode (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const void *digest_buf, MAYBE_UNUSED const salt_t *salt, MAYBE_UNUSED const void *esalt_buf, MAYBE_UNUSED const void *hook_salt_buf, MAYBE_UNUSED const hashinfo_t *hash_info, char *line_buf, MAYBE_UNUSED const int line_size)
{
  int line_len = 0;

  const pdf_t *pdf = (const pdf_t *) esalt_buf;

  if (pdf->id_len == 32)
  {
    const char *line_format = "$pdf$%d*%d*%d*%u*%d*%d*%08x%08x%08x%08x%08x%08x%08x%08x*%d*%08x%08x%08x%08x%08x%08x%08x%08x*%d*%08x%08x%08x%08x%08x%08x%08x%08x%s";

    if (pdf->P_minus == 1) line_format = "$pdf$%d*%d*%d*%d*%d*%d*%08x%08x%08x%08x%08x%08x%08x%08x*%d*%08x%08x%08x%08x%08x%08x%08x%08x*%d*%08x%08x%08x%08x%08x%08x%08x%08x%s";

    line_len = snprintf (line_buf, line_size, line_format,
      pdf->V,
      pdf->R,
      128,
      pdf->P,
      pdf->enc_md,
      pdf->id_len,
      byte_swap_32 (pdf->id_buf[0]),
      byte_swap_32 (pdf->id_buf[1]),
      byte_swap_32 (pdf->id_buf[2]),
      byte_swap_32 (pdf->id_buf[3]),
      byte_swap_32 (pdf->id_buf[4]),
      byte_swap_32 (pdf->id_buf[5]),
      byte_swap_32 (pdf->id_buf[6]),
      byte_swap_32 (pdf->id_buf[7]),
      pdf->u_len,
      byte_swap_32 (pdf->u_buf[0]),
      byte_swap_32 (pdf->u_buf[1]),
      byte_swap_32 (pdf->u_buf[2]),
      byte_swap_32 (pdf->u_buf[3]),
      byte_swap_32 (pdf->u_buf[4]),
      byte_swap_32 (pdf->u_buf[5]),
      byte_swap_32 (pdf->u_buf[6]),
      byte_swap_32 (pdf->u_buf[7]),
      pdf->o_len,
      byte_swap_32 (pdf->o_buf[0]),
      byte_swap_32 (pdf->o_buf[1]),
      byte_swap_32 (pdf->o_buf[2]),
      byte_swap_32 (pdf->o_buf[3]),
      byte_swap_32 (pdf->o_buf[4]),
      byte_swap_32 (pdf->o_buf[5]),
      byte_swap_32 (pdf->o_buf[6]),
      byte_swap_32 (pdf->o_buf[7]),
      (const char *) pdf->u_pass_buf // TODO just prints the old hash now, we don't edit the hash to add a recovered user-password to it (yet)
    );
  }
  else
  {
    const char *line_format = "$pdf$%d*%d*%d*%u*%d*%d*%08x%08x%08x%08x*%d*%08x%08x%08x%08x%08x%08x%08x%08x*%d*%08x%08x%08x%08x%08x%08x%08x%08x%s";

    if (pdf->P_minus == 1) line_format = "$pdf$%d*%d*%d*%d*%d*%d*%08x%08x%08x%08x*%d*%08x%08x%08x%08x%08x%08x%08x%08x*%d*%08x%08x%08x%08x%08x%08x%08x%08x%s";

    line_len = snprintf (line_buf, line_size, line_format,
      pdf->V,
      pdf->R,
      128,
      pdf->P,
      pdf->enc_md,
      pdf->id_len,
      byte_swap_32 (pdf->id_buf[0]),
      byte_swap_32 (pdf->id_buf[1]),
      byte_swap_32 (pdf->id_buf[2]),
      byte_swap_32 (pdf->id_buf[3]),
      pdf->u_len,
      byte_swap_32 (pdf->u_buf[0]),
      byte_swap_32 (pdf->u_buf[1]),
      byte_swap_32 (pdf->u_buf[2]),
      byte_swap_32 (pdf->u_buf[3]),
      byte_swap_32 (pdf->u_buf[4]),
      byte_swap_32 (pdf->u_buf[5]),
      byte_swap_32 (pdf->u_buf[6]),
      byte_swap_32 (pdf->u_buf[7]),
      pdf->o_len,
      byte_swap_32 (pdf->o_buf[0]),
      byte_swap_32 (pdf->o_buf[1]),
      byte_swap_32 (pdf->o_buf[2]),
      byte_swap_32 (pdf->o_buf[3]),
      byte_swap_32 (pdf->o_buf[4]),
      byte_swap_32 (pdf->o_buf[5]),
      byte_swap_32 (pdf->o_buf[6]),
      byte_swap_32 (pdf->o_buf[7]),
      (const char *) pdf->u_pass_buf // TODO just prints the old hash now, we don't edit the hash to add a recovered user-password to it (yet)
    );
  }

  return line_len;
}

void module_init (module_ctx_t *module_ctx)
{
  module_ctx->module_context_size             = MODULE_CONTEXT_SIZE_CURRENT;
  module_ctx->module_interface_version        = MODULE_INTERFACE_VERSION_CURRENT;

  module_ctx->module_attack_exec              = module_attack_exec;
  module_ctx->module_benchmark_esalt          = MODULE_DEFAULT;
  module_ctx->module_benchmark_hook_salt      = MODULE_DEFAULT;
  module_ctx->module_benchmark_mask           = MODULE_DEFAULT;
  module_ctx->module_benchmark_charset        = MODULE_DEFAULT;
  module_ctx->module_benchmark_salt           = MODULE_DEFAULT;
  module_ctx->module_bridge_name              = MODULE_DEFAULT;
  module_ctx->module_bridge_type              = MODULE_DEFAULT;
  module_ctx->module_build_plain_postprocess  = module_build_plain_postprocess;
  module_ctx->module_deep_comp_kernel         = MODULE_DEFAULT;
  module_ctx->module_deprecated_notice        = MODULE_DEFAULT;
  module_ctx->module_dgst_pos0                = module_dgst_pos0;
  module_ctx->module_dgst_pos1                = module_dgst_pos1;
  module_ctx->module_dgst_pos2                = module_dgst_pos2;
  module_ctx->module_dgst_pos3                = module_dgst_pos3;
  module_ctx->module_dgst_size                = module_dgst_size;
  module_ctx->module_dictstat_disable         = MODULE_DEFAULT;
  module_ctx->module_esalt_size               = module_esalt_size;
  module_ctx->module_extra_buffer_size        = MODULE_DEFAULT;
  module_ctx->module_extra_tmp_size           = MODULE_DEFAULT;
  module_ctx->module_extra_tuningdb_block     = MODULE_DEFAULT;
  module_ctx->module_forced_outfile_format    = MODULE_DEFAULT;
  module_ctx->module_hash_binary_count        = MODULE_DEFAULT;
  module_ctx->module_hash_binary_parse        = MODULE_DEFAULT;
  module_ctx->module_hash_binary_save         = MODULE_DEFAULT;
  module_ctx->module_hash_decode_postprocess  = MODULE_DEFAULT;
  module_ctx->module_hash_decode_potfile      = MODULE_DEFAULT;
  module_ctx->module_hash_decode_zero_hash    = MODULE_DEFAULT;
  module_ctx->module_hash_decode              = module_hash_decode;
  module_ctx->module_hash_encode_status       = MODULE_DEFAULT;
  module_ctx->module_hash_encode_potfile      = MODULE_DEFAULT;
  module_ctx->module_hash_encode              = module_hash_encode;
  module_ctx->module_hash_init_selftest       = MODULE_DEFAULT;
  module_ctx->module_hash_mode                = MODULE_DEFAULT;
  module_ctx->module_hash_category            = module_hash_category;
  module_ctx->module_hash_name                = module_hash_name;
  module_ctx->module_hashes_count_min         = MODULE_DEFAULT;
  module_ctx->module_hashes_count_max         = MODULE_DEFAULT;
  module_ctx->module_hlfmt_disable            = MODULE_DEFAULT;
  module_ctx->module_hook_extra_param_size    = MODULE_DEFAULT;
  module_ctx->module_hook_extra_param_init    = MODULE_DEFAULT;
  module_ctx->module_hook_extra_param_term    = MODULE_DEFAULT;
  module_ctx->module_hook12                   = MODULE_DEFAULT;
  module_ctx->module_hook23                   = MODULE_DEFAULT;
  module_ctx->module_hook_salt_size           = MODULE_DEFAULT;
  module_ctx->module_hook_size                = MODULE_DEFAULT;
  module_ctx->module_jit_build_options        = module_jit_build_options;
  module_ctx->module_jit_cache_disable        = MODULE_DEFAULT;
  module_ctx->module_kernel_accel_max         = MODULE_DEFAULT;
  module_ctx->module_kernel_accel_min         = MODULE_DEFAULT;
  module_ctx->module_kernel_loops_max         = module_kernel_loops_max;
  module_ctx->module_kernel_loops_min         = module_kernel_loops_min;
  module_ctx->module_kernel_threads_max       = MODULE_DEFAULT;
  module_ctx->module_kernel_threads_min       = MODULE_DEFAULT;
  module_ctx->module_kern_type                = module_kern_type;
  module_ctx->module_kern_type_dynamic        = MODULE_DEFAULT;
  module_ctx->module_opti_type                = module_opti_type;
  module_ctx->module_opts_type                = module_opts_type;
  module_ctx->module_outfile_check_disable    = MODULE_DEFAULT;
  module_ctx->module_outfile_check_nocomp     = MODULE_DEFAULT;
  module_ctx->module_potfile_custom_check     = MODULE_DEFAULT;
  module_ctx->module_potfile_disable          = MODULE_DEFAULT;
  module_ctx->module_potfile_keep_all_hashes  = MODULE_DEFAULT;
  module_ctx->module_pwdump_column            = MODULE_DEFAULT;
  module_ctx->module_pw_max                   = module_pw_max;
  module_ctx->module_pw_min                   = MODULE_DEFAULT;
  module_ctx->module_salt_max                 = MODULE_DEFAULT;
  module_ctx->module_salt_min                 = MODULE_DEFAULT;
  module_ctx->module_salt_type                = module_salt_type;
  module_ctx->module_separator                = MODULE_DEFAULT;
  module_ctx->module_st_hash                  = module_st_hash;
  module_ctx->module_st_pass                  = module_st_pass;
  module_ctx->module_tmp_size                 = module_tmp_size;
  module_ctx->module_unstable_warning         = module_unstable_warning;
  module_ctx->module_warmup_disable           = MODULE_DEFAULT;
}
