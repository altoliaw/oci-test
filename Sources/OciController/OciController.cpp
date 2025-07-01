#include "../../Headers/OciController/OciController.hpp"
void OciController::start() {
    char* sql = 
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
    std::vector<TestDataModel> models;
    for (int i = 0; i < 10; i++ ) {
        TestDataModel temp;
        models.push_back(temp);
    }
    DBSql::BatchInsert(sql, models, 50);
}