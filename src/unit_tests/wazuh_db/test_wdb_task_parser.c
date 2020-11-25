
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>

#include "hash_op.h"
#include "os_err.h"
#include "wazuh_db/wdb.h"
#include "../wrappers/wazuh/shared/debug_op_wrappers.h"
#include "../wrappers/wazuh/wazuh_db/wdb_wrappers.h"
#include "../wrappers/wazuh/wazuh_db/wdb_task_wrappers.h"
#include "../wrappers/externals/sqlite/sqlite3_wrappers.h"
#include "wazuhdb_op.h"

// Setup/teardown

static int teardown_json(void **state) {
    cJSON *json = *state;
    cJSON_Delete(json);
    return 0;
}

// Tests

void test_wdb_parse_task_upgrade_ok(void **state)
{
    int agent_id = 15;
    char *node = "master";
    char *module = "api";
    char *command = "upgrade";
    int task_id = 31;

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);
    cJSON_AddStringToObject(parameters, "node", node);
    cJSON_AddStringToObject(parameters, "module", module);

    expect_value(__wrap_wdb_task_insert_task, agent_id, agent_id);
    expect_string(__wrap_wdb_task_insert_task, node, node);
    expect_string(__wrap_wdb_task_insert_task, module, module);
    expect_string(__wrap_wdb_task_insert_task, command, command);
    will_return(__wrap_wdb_task_insert_task, task_id);

    int result = wdb_parse_task_upgrade((wdb_t*)1, parameters, command, output);

    *state = (void*)parameters;

    assert_int_equal(result, 0);
    assert_string_equal(output, "ok {\"error\":0,\"task_id\":31}");
}

void test_wdb_parse_task_upgrade_err(void **state)
{
    int agent_id = 15;
    char *node = "master";
    char *module = "api";
    char *command = "upgrade";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);
    cJSON_AddStringToObject(parameters, "node", node);
    cJSON_AddStringToObject(parameters, "module", module);

    expect_value(__wrap_wdb_task_insert_task, agent_id, agent_id);
    expect_string(__wrap_wdb_task_insert_task, node, node);
    expect_string(__wrap_wdb_task_insert_task, module, module);
    expect_string(__wrap_wdb_task_insert_task, command, command);
    will_return(__wrap_wdb_task_insert_task, OS_INVALID);

    int result = wdb_parse_task_upgrade((wdb_t*)1, parameters, command, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "ok {\"error\":-1}");
}

void test_wdb_parse_task_upgrade_module_err(void **state)
{
    int agent_id = 15;
    char *node = "master";
    char *command = "upgrade";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);
    cJSON_AddStringToObject(parameters, "node", node);

    int result = wdb_parse_task_upgrade((wdb_t*)1, parameters, command, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error insert task: 'parsing module error'");
}

void test_wdb_parse_task_upgrade_node_err(void **state)
{
    int agent_id = 15;
    char *module = "api";
    char *command = "upgrade";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);
    cJSON_AddStringToObject(parameters, "module", module);

    int result = wdb_parse_task_upgrade((wdb_t*)1, parameters, command, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error insert task: 'parsing node error'");
}

void test_wdb_parse_task_upgrade_agent_err(void **state)
{
    char *node = "master";
    char *module = "api";
    char *command = "upgrade";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddStringToObject(parameters, "node", node);
    cJSON_AddStringToObject(parameters, "module", module);

    int result = wdb_parse_task_upgrade((wdb_t*)1, parameters, command, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error insert task: 'parsing agent error'");
}

void test_wdb_parse_task_upgrade_get_status_ok(void **state)
{
    int agent_id = 15;
    char *node = "master";
    char *status_result = "Done";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);
    cJSON_AddStringToObject(parameters, "node", node);

    expect_value(__wrap_wdb_task_get_upgrade_task_status, agent_id, agent_id);
    expect_string(__wrap_wdb_task_get_upgrade_task_status, node, node);
    will_return(__wrap_wdb_task_get_upgrade_task_status, status_result);
    will_return(__wrap_wdb_task_get_upgrade_task_status, 0);

    int result = wdb_parse_task_upgrade_get_status((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, 0);
    assert_string_equal(output, "ok {\"error\":0,\"status\":\"Done\"}");
}

void test_wdb_parse_task_upgrade_get_status_err(void **state)
{
    int agent_id = 15;
    char *node = "master";
    char *status_result = "Done";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);
    cJSON_AddStringToObject(parameters, "node", node);

    expect_value(__wrap_wdb_task_get_upgrade_task_status, agent_id, agent_id);
    expect_string(__wrap_wdb_task_get_upgrade_task_status, node, node);
    will_return(__wrap_wdb_task_get_upgrade_task_status, status_result);
    will_return(__wrap_wdb_task_get_upgrade_task_status, OS_INVALID);

    int result = wdb_parse_task_upgrade_get_status((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "ok {\"error\":-1}");
}

void test_wdb_parse_task_upgrade_get_status_node_err(void **state)
{
    int agent_id = 15;

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);

    int result = wdb_parse_task_upgrade_get_status((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error get upgrade task status: 'parsing node error'");
}

void test_wdb_parse_task_upgrade_get_status_agent_err(void **state)
{
    char *node = "master";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddStringToObject(parameters, "node", node);

    int result = wdb_parse_task_upgrade_get_status((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error get upgrade task status: 'parsing agent error'");
}

void test_wdb_parse_task_upgrade_update_status_ok(void **state)
{
    int agent_id = 15;
    char *node = "master";
    char *status = "Failed";
    char *error_msg = "Invalid upgrade";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);
    cJSON_AddStringToObject(parameters, "node", node);
    cJSON_AddStringToObject(parameters, "status", status);
    cJSON_AddStringToObject(parameters, "error_msg", error_msg);

    expect_value(__wrap_wdb_task_update_upgrade_task_status, agent_id, agent_id);
    expect_string(__wrap_wdb_task_update_upgrade_task_status, node, node);
    expect_string(__wrap_wdb_task_update_upgrade_task_status, status, status);
    expect_string(__wrap_wdb_task_update_upgrade_task_status, error, error_msg);
    will_return(__wrap_wdb_task_update_upgrade_task_status, 0);

    int result = wdb_parse_task_upgrade_update_status((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, 0);
    assert_string_equal(output, "ok {\"error\":0}");
}

void test_wdb_parse_task_upgrade_update_status_err(void **state)
{
    int agent_id = 15;
    char *node = "master";
    char *status = "Failed";
    char *error_msg = "Invalid upgrade";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);
    cJSON_AddStringToObject(parameters, "node", node);
    cJSON_AddStringToObject(parameters, "status", status);
    cJSON_AddStringToObject(parameters, "error_msg", error_msg);

    expect_value(__wrap_wdb_task_update_upgrade_task_status, agent_id, agent_id);
    expect_string(__wrap_wdb_task_update_upgrade_task_status, node, node);
    expect_string(__wrap_wdb_task_update_upgrade_task_status, status, status);
    expect_string(__wrap_wdb_task_update_upgrade_task_status, error, error_msg);
    will_return(__wrap_wdb_task_update_upgrade_task_status, OS_INVALID);

    int result = wdb_parse_task_upgrade_update_status((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "ok {\"error\":-1}");
}

void test_wdb_parse_task_upgrade_update_status_status_err(void **state)
{
    int agent_id = 15;
    char *node = "master";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);
    cJSON_AddStringToObject(parameters, "node", node);

    int result = wdb_parse_task_upgrade_update_status((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error upgrade update status task: 'parsing status error'");
}

void test_wdb_parse_task_upgrade_update_status_node_err(void **state)
{
    int agent_id = 15;
    char *status = "Failed";
    char *error_msg = "Invalid upgrade";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);
    cJSON_AddStringToObject(parameters, "status", status);
    cJSON_AddStringToObject(parameters, "error_msg", error_msg);

    int result = wdb_parse_task_upgrade_update_status((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error upgrade update status task: 'parsing node error'");
}

void test_wdb_parse_task_upgrade_update_status_agent_err(void **state)
{
    char *node = "master";
    char *status = "Failed";
    char *error_msg = "Invalid upgrade";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddStringToObject(parameters, "node", node);
    cJSON_AddStringToObject(parameters, "status", status);
    cJSON_AddStringToObject(parameters, "error_msg", error_msg);

    int result = wdb_parse_task_upgrade_update_status((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error upgrade update status task: 'parsing agent error'");
}

void test_wdb_parse_task_upgrade_result_ok(void **state)
{
    int agent_id = 15;
    char *node_result = "master";
    char *module_result = "upgrade_module";
    char *command_result = "upgrade_custom";
    char *status_result = "Pending";
    char *error_result = "Error message";
    int create_time = 123456;
    int update_time = 123465;
    int task_id = 44;

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);

    expect_value(__wrap_wdb_task_get_upgrade_task_by_agent_id, agent_id, agent_id);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, node_result);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, module_result);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, command_result);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, status_result);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, error_result);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, create_time);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, update_time);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, task_id);

    int result = wdb_parse_task_upgrade_result((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, 0);
    assert_string_equal(output, "ok {\"error\":0,\"task_id\":44,\"node\":\"master\",\"module\":\"upgrade_module\",\"command\":\"upgrade_custom\",\"status\":\"Pending\",\"error_msg\":\"Error message\",\"create_time\":123456,\"update_time\":123465}");
}

void test_wdb_parse_task_upgrade_result_err(void **state)
{
    int agent_id = 15;
    char *node_result = "master";
    char *module_result = "upgrade_module";
    char *command_result = "upgrade_custom";
    char *status_result = "Pending";
    char *error_result = "Error message";
    int create_time = 123456;
    int update_time = 123465;

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "agent", agent_id);

    expect_value(__wrap_wdb_task_get_upgrade_task_by_agent_id, agent_id, agent_id);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, node_result);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, module_result);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, command_result);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, status_result);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, error_result);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, create_time);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, update_time);
    will_return(__wrap_wdb_task_get_upgrade_task_by_agent_id, OS_INVALID);

    int result = wdb_parse_task_upgrade_result((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "ok {\"error\":-1}");
}

void test_wdb_parse_task_upgrade_result_agent_err(void **state)
{
    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();

    int result = wdb_parse_task_upgrade_result((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error upgrade result task: 'parsing agent error'");
}

void test_wdb_parse_task_upgrade_cancel_tasks_ok(void **state)
{
    char *node = "master";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddStringToObject(parameters, "node", node);

    expect_string(__wrap_wdb_task_cancel_upgrade_tasks, node, node);
    will_return(__wrap_wdb_task_cancel_upgrade_tasks, 0);

    int result = wdb_parse_task_upgrade_cancel_tasks((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, 0);
    assert_string_equal(output, "ok {\"error\":0}");
}

void test_wdb_parse_task_upgrade_cancel_tasks_err(void **state)
{
    char *node = "master";

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddStringToObject(parameters, "node", node);

    expect_string(__wrap_wdb_task_cancel_upgrade_tasks, node, node);
    will_return(__wrap_wdb_task_cancel_upgrade_tasks, OS_INVALID);

    int result = wdb_parse_task_upgrade_cancel_tasks((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "ok {\"error\":-1}");
}

void test_wdb_parse_task_upgrade_cancel_tasks_agent_err(void **state)
{
    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();

    int result = wdb_parse_task_upgrade_cancel_tasks((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error upgrade cancel task: 'parsing node error'");
}

void test_wdb_parse_task_set_timeout_ok(void **state)
{
    int now = 12345;
    int interval = 100;
    int next_timeout = 12445;

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "now", now);
    cJSON_AddNumberToObject(parameters, "interval", interval);

    expect_value(__wrap_wdb_task_set_timeout_status, now, now);
    expect_value(__wrap_wdb_task_set_timeout_status, interval, interval);
    will_return(__wrap_wdb_task_set_timeout_status, next_timeout);
    will_return(__wrap_wdb_task_set_timeout_status, 0);

    int result = wdb_parse_task_set_timeout((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, 0);
    assert_string_equal(output, "ok {\"error\":0,\"timestamp\":12445}");
}

void test_wdb_parse_task_set_timeout_err(void **state)
{
    int now = 12345;
    int interval = 100;
    int next_timeout = 12445;

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "now", now);
    cJSON_AddNumberToObject(parameters, "interval", interval);

    expect_value(__wrap_wdb_task_set_timeout_status, now, now);
    expect_value(__wrap_wdb_task_set_timeout_status, interval, interval);
    will_return(__wrap_wdb_task_set_timeout_status, next_timeout);
    will_return(__wrap_wdb_task_set_timeout_status, OS_INVALID);

    int result = wdb_parse_task_set_timeout((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "ok {\"error\":-1}");
}

void test_wdb_parse_task_set_timeout_interval_err(void **state)
{
    int now = 12345;

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "now", now);

    int result = wdb_parse_task_set_timeout((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error set timeout task: 'parsing interval error'");
}

void test_wdb_parse_task_set_timeout_now_err(void **state)
{
    int interval = 100;

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "interval", interval);

    int result = wdb_parse_task_set_timeout((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error set timeout task: 'parsing now error'");
}

void test_wdb_parse_task_delete_old_ok(void **state)
{
    int timestamp = 12345;

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "timestamp", timestamp);

    expect_value(__wrap_wdb_task_delete_old_entries, timestamp, timestamp);
    will_return(__wrap_wdb_task_delete_old_entries, 0);

    int result = wdb_parse_task_delete_old((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, 0);
    assert_string_equal(output, "ok {\"error\":0}");
}

void test_wdb_parse_task_delete_old_err(void **state)
{
    int timestamp = 12345;

    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();
    cJSON_AddNumberToObject(parameters, "timestamp", timestamp);

    expect_value(__wrap_wdb_task_delete_old_entries, timestamp, timestamp);
    will_return(__wrap_wdb_task_delete_old_entries, OS_INVALID);

    int result = wdb_parse_task_delete_old((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "ok {\"error\":-1}");
}

void test_wdb_parse_task_delete_old_timestamp_err(void **state)
{
    char output[OS_MAXSTR + 1];
    *output = '\0';

    cJSON *parameters = cJSON_CreateObject();

    int result = wdb_parse_task_delete_old((wdb_t*)1, parameters, output);

    *state = (void*)parameters;

    assert_int_equal(result, OS_INVALID);
    assert_string_equal(output, "err Error delete old task: 'parsing timestamp error'");
}

int main()
{
    const struct CMUnitTest tests[] = {
        // wdb_parse_task_upgrade
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_ok, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_module_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_node_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_agent_err, teardown_json),
        // wdb_parse_task_upgrade_get_status
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_get_status_ok, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_get_status_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_get_status_node_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_get_status_agent_err, teardown_json),
        // wdb_parse_task_upgrade_update_status
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_update_status_ok, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_update_status_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_update_status_status_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_update_status_node_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_update_status_agent_err, teardown_json),
        // wdb_parse_task_upgrade_result
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_result_ok, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_result_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_result_agent_err, teardown_json),
        // wdb_parse_task_upgrade_cancel_tasks
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_cancel_tasks_ok, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_cancel_tasks_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_upgrade_cancel_tasks_agent_err, teardown_json),
        // wdb_parse_task_set_timeout
        cmocka_unit_test_teardown(test_wdb_parse_task_set_timeout_ok, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_set_timeout_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_set_timeout_interval_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_set_timeout_now_err, teardown_json),
        // wdb_parse_task_delete_old
        cmocka_unit_test_teardown(test_wdb_parse_task_delete_old_ok, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_delete_old_err, teardown_json),
        cmocka_unit_test_teardown(test_wdb_parse_task_delete_old_timestamp_err, teardown_json)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}