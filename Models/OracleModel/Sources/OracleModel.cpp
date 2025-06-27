#include <iostream>
#include <cstring>
#include <string>
#include "../Headers/OracleModel.hpp"


bool OracleModel::Initialize(){
    // Initialize OCILIB
    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        printf("Failed to initialize OCILIB\n");
        return false;
    }
    return true;
}

bool OracleModel::Connection(){
    // Create a connection to the Oracle database
    cn = OCI_ConnectionCreate("192.168.30.178/orcl", "cindyguo", "113598013kuo", OCI_SESSION_DEFAULT);
    if (!cn)
    {
        std::cout << "Connection failed!" << std::endl;
        return false;
    }

    std::cout << "Connection established successfully!" << std::endl;

    // Start transaction manually
    OCI_SetAutoCommit(cn, FALSE);

    return true;
}

bool OracleModel::PrepareStatement(const char* sql)
{
    // Create the statement
    st = OCI_StatementCreate(cn);

    // Prepare the statement
    success = OCI_Prepare(st, sql);
    if (!success)
    {
        std::cout << "Failed to prepare statement." << std::endl;
        return false;
    }
    return true;
}

void OracleModel::bindData()
{
    #define SIZE_OFSOMEFDJF 80
    OCI_BindArraySetSize(st, SIZE_OFSOMEFDJF);
    char* fakedata = "extremely_long_value_of_column_with_extremely_long_name_number"; //remove const
    char* value[SIZE_OFSOMEFDJF]; 
    for (int i = 0; i<SIZE_OFSOMEFDJF; ++i) {
        value[i] = fakedata;
    }
    for (int i = 1; i <= 50; ++i) {
        char fmtstr[100];
        snprintf(fmtstr, sizeof(fmtstr), ":%d", i);
        OCI_BindArrayOfStrings(st, fmtstr, (otext*)value, sizeof(fakedata), SIZE_OFSOMEFDJF); //add (otext*)
    }

}

bool OracleModel::SQLExecution(){
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
    

    if(!PrepareStatement(sql)) return false;
    
    bindData();

    // Execute the batch
    if (OCI_Execute(st))
    {
        std::cout << "PreparedStatement Batch executed, INSERT done" << std::endl;
        OCI_Commit(cn);
    }
    else
    {
        std::cout << "Batch execution failed. Rolling back." << std::endl;
        OCI_Rollback(cn);
        return false;
    }

    return true;
}

bool OracleModel::Cleanup()
{
    if(st != nullptr)
        OCI_StatementFree(st);
    if(cn!= nullptr)
        OCI_ConnectionFree(cn);
    OCI_Cleanup();
    
    return true;
}

void OracleModel::err_handler(OCI_Error *err)
{
    std::cout << OCI_ErrorGetString(err) << std::endl;
}
