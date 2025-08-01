/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#include "common.h"
#include "types.h"
#include "modules.h"
#include "bitops.h"
#include "convert.h"
#include "shared.h"
#include "memory.h"

static const u32   ATTACK_EXEC    = ATTACK_EXEC_OUTSIDE_KERNEL;
static const u32   DGST_POS0      = 0;
static const u32   DGST_POS1      = 1;
static const u32   DGST_POS2      = 2;
static const u32   DGST_POS3      = 3;
static const u32   DGST_SIZE      = DGST_SIZE_4_4;
static const u32   HASH_CATEGORY  = HASH_CATEGORY_OS;
static const char *HASH_NAME      = "DPAPI masterkey file v1 (context 3)";
static const u64   KERN_TYPE      = 15310;
static const u32   OPTI_TYPE      = OPTI_TYPE_ZERO_BYTE
                                  | OPTI_TYPE_SLOW_HASH_SIMD_LOOP
                                  | OPTI_TYPE_SLOW_HASH_SIMD_LOOP2;
static const u64   OPTS_TYPE      = OPTS_TYPE_STOCK_MODULE
                                  | OPTS_TYPE_PT_GENERATE_LE
                                  | OPTS_TYPE_INIT2
                                  | OPTS_TYPE_LOOP2;
static const u32   SALT_TYPE      = SALT_TYPE_EMBEDDED;
static const char *ST_PASS        = "hashcat";
static const char *ST_HASH        = "$DPAPImk$1*3*S-15-21-407415836-404165111-436049749-1915*des3*sha1*14825*3e86e7d8437c4d5582ff668a83632cb2*208*96ad763b59e67c9f5c3d925e42bbe28a1412b919d1dc4abf03b2bed4c5c244056c14931d94d441117529b7171dfd6ebbe6eecf5d958b65574c293778fbadb892351cc59d5c65d65d2fcda73f5b056548a4a5550106d03d0c39d3cca7e5cdc0d521f48ac9e51cecc5";

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

typedef struct dpapimk
{
  u32 context;

  u32 SID[32];
  u32 SID_len;
  u32 SID_offset;

  /* here only for possible
     forward compatibility
  */
  // u8 cipher_algo[16];
  // u8 hash_algo[16];

  u32 iv[4];
  u32 contents_len;
  u32 contents[128];

} dpapimk_t;

typedef struct dpapimk_tmp_v1
{
  u32 ipad[8];
  u32 opad[8];
  u32 dgst[32];
  u32 out[32];

  u32 userKey[5];

} dpapimk_tmp_v1_t;

static const char *SIGNATURE_DPAPIMK = "$DPAPImk$";

bool module_unstable_warning (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra, MAYBE_UNUSED const hc_device_param_t *device_param)
{
  if ((device_param->opencl_platform_vendor_id == VENDOR_ID_APPLE) && (device_param->opencl_device_type & CL_DEVICE_TYPE_GPU))
  {
    if (device_param->is_metal == true)
    {
      if (strncmp (device_param->device_name, "Intel", 5) == 0)
      {
        // Intel Iris Graphics, Metal Version 244.303: failed to create 'm15310_init' pipeline, timeout reached
        return true;
      }
    }
  }

  return false;
}

salt_t *module_benchmark_salt (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  salt_t *salt = (salt_t *) hcmalloc (sizeof (salt_t));

  salt->salt_iter  = 10000 - 1;
  salt->salt_iter2 = 13450 - 1;
  salt->salt_len   = 16;

  return salt;
}

u64 module_tmp_size (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u64 tmp_size = (const u64) sizeof (dpapimk_tmp_v1_t);

  return tmp_size;
}

u64 module_esalt_size (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u64 esalt_size = (const u64) sizeof (dpapimk_t);

  return esalt_size;
}

int module_hash_decode (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED void *digest_buf, MAYBE_UNUSED salt_t *salt, MAYBE_UNUSED void *esalt_buf, MAYBE_UNUSED void *hook_salt_buf, MAYBE_UNUSED hashinfo_t *hash_info, const char *line_buf, MAYBE_UNUSED const int line_len)
{
  u32 *digest = (u32 *) digest_buf;

  dpapimk_t *dpapimk = (dpapimk_t *) esalt_buf;

  memset (dpapimk, 0, sizeof (dpapimk_t));

  hc_token_t token;

  memset (&token, 0, sizeof (hc_token_t));

  token.token_cnt  = 10;

  token.signatures_cnt    = 1;
  token.signatures_buf[0] = SIGNATURE_DPAPIMK;

  // signature
  token.len[0]     = 9;
  token.attr[0]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_SIGNATURE;

  // version
  token.sep[1]     = '*';
  token.len[1]     = 1;
  token.attr[1]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  // context
  token.sep[2]     = '*';
  token.len[2]     = 1;
  token.attr[2]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  // sid
  token.sep[3]     = '*';
  token.len_min[3] = 10;
  token.len_max[3] = 60;
  token.attr[3]    = TOKEN_ATTR_VERIFY_LENGTH;

  // cipher
  token.sep[4]     = '*';
  token.len_min[4] = 4;
  token.len_max[4] = 6;
  token.attr[4]    = TOKEN_ATTR_VERIFY_LENGTH;

  // hash
  token.sep[5]     = '*';
  token.len_min[5] = 4;
  token.len_max[5] = 6;
  token.attr[5]    = TOKEN_ATTR_VERIFY_LENGTH;

  // iterations
  token.sep[6]     = '*';
  token.len_min[6] = 1;
  token.len_max[6] = 6;
  token.attr[6]    = TOKEN_ATTR_VERIFY_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  // iv
  token.sep[7]     = '*';
  token.len[7]     = 32;
  token.attr[7]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_HEX;

  // content len
  token.sep[8]     = '*';
  token.len_min[8] = 1;
  token.len_max[8] = 6;
  token.attr[8]    = TOKEN_ATTR_VERIFY_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  // content
  token.len_min[9] = 0;
  token.len_max[9] = 1024;
  token.attr[9]    = TOKEN_ATTR_VERIFY_LENGTH
                   | TOKEN_ATTR_VERIFY_HEX;

  const int rc_tokenizer = input_tokenizer ((const u8 *) line_buf, line_len, &token);

  if (rc_tokenizer != PARSER_OK) return (rc_tokenizer);

  const u8 *version_pos       = token.buf[1];
  const u8 *context_pos       = token.buf[2];
  const u8 *SID_pos           = token.buf[3];
  const u8 *rounds_pos        = token.buf[6];
  const u8 *iv_pos            = token.buf[7];
  const u8 *contents_len_pos  = token.buf[8];
  const u8 *contents_pos      = token.buf[9];

  /**
   * content verification
   */

  const int version      = hc_strtoul ((const char *) version_pos,      NULL, 10);
  const int contents_len = hc_strtoul ((const char *) contents_len_pos, NULL, 10);

  if (version == 1)
  {
    if (contents_len != 208) return (PARSER_SALT_LENGTH);
  }
  else if (version == 2)
  {
    if (contents_len != 288) return (PARSER_SALT_LENGTH);
  }
  else
  {
    return (PARSER_SALT_VALUE);
  }

  if (contents_len != token.len[9]) return (PARSER_SALT_LENGTH);

  dpapimk->contents_len = contents_len;

  dpapimk->context = hc_strtoul ((const char *) context_pos, NULL, 10);

  if (dpapimk->context != 3) return (PARSER_SALT_LENGTH);

  for (u32 i = 0; i < dpapimk->contents_len / 8; i++)
  {
    dpapimk->contents[i] = hex_to_u32 (&contents_pos[i * 8]);

    dpapimk->contents[i] = byte_swap_32 (dpapimk->contents[i]);
  }

  // SID

  const int SID_len = token.len[3];

  u8 SID_utf16le[128] = { 0 };

  for (int i = 0; i < SID_len; i++)
  {
    SID_utf16le[i * 2] = SID_pos[i];
  }

  dpapimk->SID_len = SID_len * 2;

  memcpy ((u8 *) dpapimk->SID, SID_utf16le, sizeof (SID_utf16le));

  for (u32 i = 0; i < 32; i++)
  {
    dpapimk->SID[i] = byte_swap_32 (dpapimk->SID[i]);
  }

  // iv

  dpapimk->iv[0] = hex_to_u32 (&iv_pos[ 0]);
  dpapimk->iv[1] = hex_to_u32 (&iv_pos[ 8]);
  dpapimk->iv[2] = hex_to_u32 (&iv_pos[16]);
  dpapimk->iv[3] = hex_to_u32 (&iv_pos[24]);

  dpapimk->iv[0] = byte_swap_32 (dpapimk->iv[0]);
  dpapimk->iv[1] = byte_swap_32 (dpapimk->iv[1]);
  dpapimk->iv[2] = byte_swap_32 (dpapimk->iv[2]);
  dpapimk->iv[3] = byte_swap_32 (dpapimk->iv[3]);

  digest[0] = dpapimk->iv[0];
  digest[1] = dpapimk->iv[1];
  digest[2] = dpapimk->iv[2];
  digest[3] = dpapimk->iv[3];

  salt->salt_buf[0] = dpapimk->iv[0];
  salt->salt_buf[1] = dpapimk->iv[1];
  salt->salt_buf[2] = dpapimk->iv[2];
  salt->salt_buf[3] = dpapimk->iv[3];

  salt->salt_len = 16;

  // iter

  salt->salt_iter  = 10000 - 1;
  salt->salt_iter2 = hc_strtoul ((const char *) rounds_pos, NULL, 10) - 1;

  return (PARSER_OK);
}

int module_hash_encode (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const void *digest_buf, MAYBE_UNUSED const salt_t *salt, MAYBE_UNUSED const void *esalt_buf, MAYBE_UNUSED const void *hook_salt_buf, MAYBE_UNUSED const hashinfo_t *hash_info, char *line_buf, MAYBE_UNUSED const int line_size)
{
  const dpapimk_t *dpapimk = (const dpapimk_t *) esalt_buf;

  u32  version      = 1;
  u32  context      = dpapimk->context;
  u32  rounds       = salt->salt_iter2 + 1;
  u32  contents_len = dpapimk->contents_len;
  u32  SID_len      = dpapimk->SID_len;
  u32  iv_len       = 32;

  u8   cipher_algorithm[8] = { 0 };
  u8   hash_algorithm[8]   = { 0 };
  u8   SID[512]            = { 0 };
  u8  *SID_tmp             = NULL;

  const u32 *ptr_SID      = (const u32 *) dpapimk->SID;
  const u32 *ptr_iv       = (const u32 *) dpapimk->iv;
  const u32 *ptr_contents = (const u32 *) dpapimk->contents;

  u32  u32_iv[4];
  u8   iv[32 + 1];

  u32  u32_contents[36];
  u8   contents[288 + 1];

  memset (u32_iv, 0, sizeof (u32_iv));
  memset (iv,     0, sizeof (iv));

  memset (u32_contents, 0, sizeof (u32_contents));
  memset (contents,     0, sizeof (contents));

  // convert back SID

  SID_tmp = (u8 *) hcmalloc ((SID_len + 1) * sizeof (u8));

  for (u32 i = 0; i < (SID_len / 4); i++)
  {
    u8 hex[8] = { 0 };

    u32_to_hex (byte_swap_32 (ptr_SID[i]), hex);

    for (u32 j = 0, k = 0; j < 8; j += 2, k++)
    {
      SID_tmp[i * 4 + k] = hex_to_u8 (&hex[j]);
    }
  }

  // overwrite trailing 0x80

  SID_tmp[SID_len] = 0;

  for (u32 i = 0, j = 0 ; j < SID_len ; i++, j += 2)
  {
    SID[i] = SID_tmp[j];
  }

  hcfree (SID_tmp);

  for (u32 i = 0; i < iv_len / 8; i++)
  {
    u32_iv[i] = byte_swap_32 (ptr_iv[i]);

    u32_to_hex (u32_iv[i], iv +  i * 8);
  }

  iv[32] = 0;

  for (u32 i = 0; i < contents_len / 8; i++)
  {
    u32_contents[i] = byte_swap_32 (ptr_contents[i]);

    u32_to_hex (u32_contents[i], contents + i * 8);
  }

  contents[208] = 0;

  if (contents_len == 208)
  {
    memcpy (cipher_algorithm, "des3", strlen ("des3"));

    memcpy (hash_algorithm, "sha1", strlen ("sha1"));
  }

  const int line_len = snprintf (line_buf, line_size, "%s%u*%u*%s*%s*%s*%u*%s*%u*%s",
    SIGNATURE_DPAPIMK,
    version,
    context,
    SID,
    cipher_algorithm,
    hash_algorithm,
    rounds,
    iv,
    contents_len,
    contents);

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
  module_ctx->module_benchmark_salt           = module_benchmark_salt;
  module_ctx->module_bridge_name              = MODULE_DEFAULT;
  module_ctx->module_bridge_type              = MODULE_DEFAULT;
  module_ctx->module_build_plain_postprocess  = MODULE_DEFAULT;
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
  module_ctx->module_jit_build_options        = MODULE_DEFAULT;
  module_ctx->module_jit_cache_disable        = MODULE_DEFAULT;
  module_ctx->module_kernel_accel_max         = MODULE_DEFAULT;
  module_ctx->module_kernel_accel_min         = MODULE_DEFAULT;
  module_ctx->module_kernel_loops_max         = MODULE_DEFAULT;
  module_ctx->module_kernel_loops_min         = MODULE_DEFAULT;
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
  module_ctx->module_pw_max                   = MODULE_DEFAULT;
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
