// ref: https://github.com/vrogier/ocilib/issues/349
#include <ocilib.h>
#include <stdio.h>
#include <stdlib.h>

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    boolean success;

    // Initialize OCILIB
    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        printf("Failed to initialize OCILIB\n");
        return EXIT_FAILURE;
    }

    // Connect to the database
    cn = OCI_ConnectionCreate("192.168.150.173/orcl", "new_user", "new_password", OCI_SESSION_DEFAULT);
    if (!cn)
    {
        printf("Connection failed!\n");
        OCI_Cleanup();
        return EXIT_FAILURE;
    }

    printf("Connection established successfully!\n");

    // Start transaction manually
    OCI_SetAutoCommit(cn, FALSE);

    // Create the SQL string with 50 placeholders
    const char *sql = 
        "INSERT INTO wubai_table_actually_wushi ("
        "extremely_long_column_name_one, extremely_long_column_name_two, extremely_long_column_name_three, "
        "extremely_long_column_name_four, extremely_long_column_name_five, extremely_long_column_name_six, "
        "extremely_long_column_name_seven, extremely_long_column_name_eight, extremely_long_column_name_nine, "
        "extremely_long_column_name_ten, extremely_long_column_name_eleven, extremely_long_column_name_twelve, "
        "extremely_long_column_name_thirteen, extremely_long_column_name_fourteen, extremely_long_column_name_fifteen, "
        "extremely_long_column_name_sixteen, extremely_long_column_name_seventeen, extremely_long_column_name_eighteen, "
        "extremely_long_column_name_nineteen, extremely_long_column_name_twenty, extremely_long_column_name_twenty_one, "
        "extremely_long_column_name_twenty_two, extremely_long_column_name_twenty_three, extremely_long_column_name_twenty_four, "
        "extremely_long_column_name_twenty_five, extremely_long_column_name_twenty_six, extremely_long_column_name_twenty_seven, "
        "extremely_long_column_name_twenty_eight, extremely_long_column_name_twenty_nine, extremely_long_column_name_thirty, "
        "extremely_long_column_name_thirty_one, extremely_long_column_name_thirty_two, extremely_long_column_name_thirty_three, "
        "extremely_long_column_name_thirty_four, extremely_long_column_name_thirty_five, extremely_long_column_name_thirty_six, "
        "extremely_long_column_name_thirty_seven, extremely_long_column_name_thirty_eight, extremely_long_column_name_thirty_nine, "
        "extremely_long_column_name_forty, extremely_long_column_name_forty_one, extremely_long_column_name_forty_two, "
        "extremely_long_column_name_forty_three, extremely_long_column_name_forty_four, extremely_long_column_name_forty_five, "
        "extremely_long_column_name_forty_six, extremely_long_column_name_forty_seven, extremely_long_column_name_forty_eight, "
        "extremely_long_column_name_forty_nine, extremely_long_column_name_fifty) "
        "VALUES (:1,:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,"
        ":21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,"
        ":41,:42,:43,:44,:45,:46,:47,:48,:49,:50)";

    // Create the statement
    st = OCI_StatementCreate(cn);

    // Prepare the statement
    success = OCI_Prepare(st, sql);
    if (!success)
    {
        printf("Failed to prepare statement.\n");
        OCI_Cleanup();
        return EXIT_FAILURE;
    }

    #define SIZE_OFSOMEFDJF 80
    OCI_BindArraySetSize(st, SIZE_OFSOMEFDJF);
    const char* fakedata = "extremely_long_value_of_column_with_extremely_long_name_number";
    char* value[SIZE_OFSOMEFDJF];
    for (int i = 0; i<SIZE_OFSOMEFDJF; ++i) {
        value[i] = fakedata;
    }
    for (int i = 1; i <= 50; ++i) {
        char fmtstr[100];
        snprintf(fmtstr, sizeof(fmtstr), ":%d", i);
        OCI_BindArrayOfStrings(st, fmtstr, value, sizeof(fakedata), SIZE_OFSOMEFDJF);
    }

    // Execute the batch
    if (OCI_Execute(st))
    {
        printf("PreparedStatement Batch executed, INSERT done\n");
        OCI_Commit(cn);
    }
    else
    {
        printf("Batch execution failed. Rolling back.\n");
        OCI_Rollback(cn);
    }

    // Cleanup
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
