#include "error_axr.h"

#include "version.h"

NODISCARD static error get_message(int const code, struct NATIVE_STR *const message) {
  switch (code) {
  case err_axr_unsupported_aviutl_version:
    return scpy(message, NSTR("AviUtl版本超出范围。"));
  case err_axr_exedit_not_found:
    return scpy(message, NSTR("找不到扩展编辑插件。"));
  case err_axr_exedit_not_found_in_same_dir:
    return scpy(message, NSTR("无法在同级目录中找到扩展编辑插件。"));
  case err_axr_unsupported_exedit_version:
    return scpy(message, NSTR("扩展编辑插件版本超出范围。"));
  case err_axr_project_is_not_open:
    return scpy(message, NSTR("AviUtl工程文件(*.aup)尚未打开。"));
  case err_axr_project_has_not_yet_been_saved:
    return scpy(message, NSTR("AviUtl工程文件(*.aup)尚未保存。"));

  case err_axr_wav_size_limit_exceeded:
    return scpy(message,
                NSTR("wav文件大小超过4gb上限，无法继续进行处理。"));
  }
  return scpy(message, NSTR("未知的错误。"));
}

NODISCARD error error_axr_init(void) {
  error err = error_register_message_mapper(err_type_axr, get_message);
  if (efailed(err)) {
    return err;
  }
  return eok();
}

NODISCARD static error build_error_message(error e, wchar_t const *const main_message, struct wstr *const dest) {
  struct wstr tmp = {0};
  struct wstr msg = {0};
  error err = eok();
  if (e == NULL) {
    err = scpy(dest, main_message);
    if (efailed(err)) {
      err = ethru(err);
      goto cleanup;
    }
    goto cleanup;
  }
  err = error_to_string(e, &tmp);
  if (efailed(err)) {
    err = ethru(err);
    goto cleanup;
  }
  err = scpym(&msg, main_message, L"\r\n\r\n", tmp.ptr);
  if (efailed(err)) {
    err = ethru(err);
    goto cleanup;
  }
  err = scpy(dest, msg.ptr);
  if (efailed(err)) {
    err = ethru(err);
    goto cleanup;
  }

cleanup:
  ereport(sfree(&msg));
  ereport(sfree(&tmp));
  return err;
}

void error_message_box(error e, HWND const window, wchar_t const *const msg) {
  struct wstr errmsg = {0};
  error err = build_error_message(e, msg, &errmsg);
  if (efailed(err)) {
    err = ethru(err);
    goto cleanup;
  }
  message_box(window, errmsg.ptr, L"通道条" VERSION_WIDE, MB_ICONERROR);

cleanup:
  ereport(sfree(&errmsg));
  if (efailed(err)) {
    ereportmsg(err, &native_unmanaged(NSTR("错误对话框显示失败。")));
  }
  ereport(e);
}
