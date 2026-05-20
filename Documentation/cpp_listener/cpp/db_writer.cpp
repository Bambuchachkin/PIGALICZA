#include "db_writer.h"

db_writer::db_writer() : con(nullptr) {
    driver = sql::mysql::get_mysql_driver_instance();
    con.reset(driver->connect(server, username, password));
    con->setSchema(database);
    std::cout << "Connecting MySQL SUCCESS" << std::endl;
}

void db_writer::process_command(nlohmann::json command){
    std::string user_id;
    std::unique_ptr<sql::PreparedStatement> get_user(con->prepareStatement("SELECT id FROM users WHERE verification_code = ?"));
    get_user->setString(1, command["v_code"].get<std::string>());
    std::unique_ptr<sql::ResultSet> user_res(get_user->executeQuery());
    if (user_res->next()) { 
        user_id = user_res->getString("id");
    } else {
        std::cerr << "User not found for v_code: " << command["v_code"] << std::endl;
        return;
    }

    if (command["type"] == "record"){
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO tanks_records (user_id, score, time, created_at) VALUES (?, ?, ?, NOW())"));
        pstmt->setString(1, user_id);
        pstmt->setString(2, command["t_s"].get<std::string>());
        pstmt->setString(3, command["t_t"].get<std::string>());
        pstmt->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> pstmt_1(con->prepareStatement("INSERT INTO races_records (user_id, score, time, created_at) VALUES (?, ?, ?, NOW())"));
        pstmt_1->setString(1, user_id);
        pstmt_1->setString(2, command["r_s"].get<std::string>());
        pstmt_1->setString(3, command["r_t"].get<std::string>());
        pstmt_1->executeUpdate();
    }
}