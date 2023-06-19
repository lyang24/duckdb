#include "../common.h"
#include <iostream>

using namespace odbc_test;

TEST_CASE("diagnostics", "[odbc]") {
	SQLRETURN ret;
	SQLHANDLE env;
	SQLHANDLE dbc;

	HSTMT hstmt = SQL_NULL_HSTMT;

	// Connect to the database using SQLConnect
	CONNECT_TO_DATABASE(env, dbc);

	// Allocate a statement handle
	ExecuteCmdAndCheckODBC("SQLAllocHandle (HSTMT)", SQLAllocHandle, SQL_HANDLE_STMT, dbc, &hstmt);

	/* TEST 1: Execute a query that will fail and check the diagnostics */
	// Execute a query that will fail
	ret = SQLExecDirect(hstmt, ConvertToSQLCHAR("this is not a valid query"), SQL_NTS);
	if (ret != SQL_ERROR) {
		FAIL("SQLExecDirect should have failed because the query is invalid");
	}

	// Get the diagnostics
	string first_state;
	string first_message;
	ACCESS_DIAGNOSTIC(first_state, first_message, hstmt, SQL_HANDLE_STMT);

	// Get the diagnostics again to make sure they are the same
	string second_state;
	string second_message;
	ACCESS_DIAGNOSTIC(second_state, second_message, hstmt, SQL_HANDLE_STMT);

	// Compare the diagnostics to make sure they are the same and that SQLGetDiagRec does not change the state of the
	// statement
	REQUIRE(::strcmp(first_state.c_str(), second_state.c_str()) == 0);
	REQUIRE(::strcmp(first_message.c_str(), second_message.c_str()) == 0);

	/* TEST 2: Test a veeery long error message */
	// Create and fill a string with 1000 characters
	string long_string;
	for (int i = 0; i < 1000; i++) {
		long_string += "x";
	}
	long_string += "END";

	// Execute a query that will fail
	ret = SQLExecDirect(hstmt, ConvertToSQLCHAR(long_string.c_str()), SQL_NTS);
	if (ret != SQL_ERROR) {
		FAIL("SQLExecDirect should have failed because the query is invalid and too long");
	}

	// Get the diagnostics
	string long_state;
	string long_message;
	ACCESS_DIAGNOSTIC(long_state, long_message, hstmt, SQL_HANDLE_STMT);

	REQUIRE(long_message.length() > 0);
	REQUIRE(long_state == "42000");

	/* TEST 3: Test SQLEndTran without a transaction */
	ret = SQLEndTran(SQL_HANDLE_DBC, dbc, SQL_ROLLBACK);
	if (ret != SQL_ERROR) {
		FAIL("SQLEndTran should have failed because there is no transaction");
	}

	// Get the diagnostics
	string first_endtran_state;
	string first_endtran_message;
	ACCESS_DIAGNOSTIC(first_endtran_state, first_endtran_message, dbc, SQL_HANDLE_DBC);

	// Get the diagnostics again to make sure they are the same
	string second_endtran_state;
	string second_endtran_message;
	ACCESS_DIAGNOSTIC(second_endtran_state, second_endtran_message, dbc, SQL_HANDLE_DBC);

	// Compare the diagnostics to make sure they are the same and that SQLGetDiagRec does not change the state of the
	// statement
	REQUIRE(first_endtran_state.compare(second_endtran_state) == 0);
	REQUIRE(first_endtran_message.compare(second_endtran_message) == 0);

	// Free the statement handle
	ExecuteCmdAndCheckODBC("SQLFreeStmt (HSTMT)", SQLFreeStmt, hstmt, SQL_CLOSE);
	ExecuteCmdAndCheckODBC("SQLFreeHandle (HSTMT)", SQLFreeHandle, SQL_HANDLE_STMT, hstmt);

	DISCONNECT_FROM_DATABASE(env, dbc);
}
