#include "privilege.h"

#include <privacy_privilege_manager.h>

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
} appdata_s;

void privilege_check_and_request_permission();
void privilege_request_response_callback(ppm_call_cause_e cause, ppm_request_result_e result, const char *privilege, void *user_data);

void privilege_check_and_request_permission() {
	const char *privilege = "http://tizen.org/privilege/healthinfo";
	ppm_check_result_e result;

	int retval = ppm_check_permission(privilege, &result);

	if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_check_permission() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in checking if an application, which calls this function, has permission to use the given privilege.", __FILE__, __func__, __LINE__);

		switch (result) {
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: The application has permission to use a privilege.", __FILE__, __func__, __LINE__);
			break;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY:
			/* Show a message and terminate the application */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: The application doesn't have permission to use a privilege.", __FILE__, __func__, __LINE__);
			break;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK:
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: The user has to be asked whether to grant permission to use a privilege.", __FILE__, __func__, __LINE__);

			retval = ppm_request_permission(privilege, privilege_request_response_callback, NULL);

			/* Log and handle errors */
			if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE) {
				dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_request_permission() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in requesting a user's response to obtain permission for using the given privilege.", __FILE__, __func__, __LINE__);
			} else {
				dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_request_permission() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to request a user's response to obtain permission for using the given privilege.", __FILE__, __func__, __LINE__);
			}
			break;
		}
	} else {
		/* retval != PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE */
		/* Handle errors */
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_check_permission() return %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to check if an application, which calls this function, has permission to use the given privilege.", __FILE__, __func__, __LINE__);
	}
}

void privilege_request_response_callback(ppm_call_cause_e cause, ppm_request_result_e result, const char *privilege, void *user_data) {
	if (cause == PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR) {
		/* Log and handle errors */
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function privilege_request_response_cb() output cause = PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR", __FILE__, __func__, __LINE__);
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Callback was called because of an error.", __FILE__, __func__, __LINE__);
	} else {
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function privilege_request_response_cb() output cause = PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ANSWER", __FILE__, __func__, __LINE__);
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Callback was called with a valid answer.", __FILE__, __func__, __LINE__);

		switch (result) {
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_ALLOW_FOREVER:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function privilege_request_response_cb() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_ALLOW_FOREVER", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: The user granted permission to use a privilege for an indefinite period of time.", __FILE__, __func__, __LINE__);
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER:
			/* Show a message and terminate the application */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function privilege_request_response_cb() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: The user denied granting permission to use a privilege for an indefinite period of time.", __FILE__, __func__, __LINE__);
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE:
			/* Show a message with explanation */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function privilege_request_response_cb() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: The user denied granting permission to use a privilege once.", __FILE__, __func__, __LINE__);
			break;
		}
	}
}

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Label */
	/* Create an actual view of the base gui.
	   Modify this part to change the view. */
	ad->label = elm_label_add(ad->conform);
	elm_object_text_set(ad->label, "<align=center>Hello Tizen</align>");
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform, ad->label);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = data;

	create_base_gui(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
	privilege_check_and_request_permission();
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}
