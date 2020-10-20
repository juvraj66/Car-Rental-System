#include<iostream>
#include<mysql.h>
#include<regex>
#include<cstring>
#include<vector>
#include<cmath>
#include<iomanip>
#include<sstream>
#include <openssl/sha.h>
#include<conio.h>
// global var
int qstate;//Checks if connection is on or off.
MYSQL* conn;
MYSQL_ROW row;
MYSQL_RES* res;
int days;// Difference between start day and end day // IS valid Dates gives the dates.
std::string user_id;
using namespace std;
class Database {
public:
    Database() {}
    int connect() {
        int status = 0;
        conn = mysql_init(0);
        conn = mysql_real_connect(conn, "localhost", "root", "", "car_rental", 3306, NULL, 0);
        if (conn) {
            status = 1; // Connection Created.
        }
        else {
            status = 0;// Connection could not be Created.
        }
        return status;
    }
    int login(int s, string ema, string pass) {
        int vaid_or_not = 0;
        if (s == 1) {// Viewing data
            string query = "Select * From C_Users where email='" + ema + "'" + " AND " + "password='" + pass + "' ;";
            const char* q = query.c_str();
            qstate = mysql_query(conn, q);
            if (!qstate) {
                res = mysql_store_result(conn);
                row = mysql_fetch_row(res);
                if (row == NULL) {
                    cout << "Account Information is incorrect Try again" << endl;
                    vaid_or_not = 0;// Fail
                }
                else {
                    cout << "--------------------Welcome " << row[3] << " ";
                    user_id = row[0];
                    cout << "To Car Rental Place------------------" << endl << endl;
                    Options();
                    vaid_or_not = 1;// Success
                }
            }
        }
        else cout << "Could Not Log In." << endl;
        return vaid_or_not;
    }
    int Create_Account(string emai, string pass, string fname, string lname) {// if user with that email exists tell the user that
        int Account_Exits = 0;
        string query = "Select * From C_Users where email='" + emai + "'" + " ;";
        string query2 = "Insert into C_Users(email, password,First_Name,Last_Name) Values('"
            + emai + "'" + ", '" + pass + "'" + ", '" + fname + "'" + ", '" + lname + "' );";
        const char* q = query.c_str();
        qstate = mysql_query(conn, q);
        if (!qstate) {
            res = mysql_store_result(conn);
            if (mysql_fetch_row(res) != NULL) {
                cout << "A user with that email already exits try again" << endl;
                Account_Exits = 1;// Acc does exit.
            }
            else {
                const char* q = query2.c_str();
                qstate = mysql_query(conn, q);
                Account_Exits = 0;// Acc does not exit
                if (!qstate) {
                    res = mysql_store_result(conn);
                    cout << "Success User Created" << endl;
                }
                else cout << "User Could Not Be Created" << endl;
            }
        }
        else "Query Failed.";
        return Account_Exits;
    }
    bool Car_Models(string state) {//User can pick a car model.
        string q1 = "Select cars.Car_ID, Car_Brand, Car_name From cars, car_info ";
        int f_length=25;
        string q2 = "where cars.car_ID = car_info.car_id && car_info.car_state = '" + state + "'";
        string query = q1 + q2;
        string query2 = "SELECT EXISTS(" + q1 + q2 + ");";
        const char* q = query2.c_str();
        qstate = mysql_query(conn, q);
        if (!qstate) {
            res = mysql_store_result(conn);
            row = mysql_fetch_row(res);
            string test = row[0];
            if (test == "0") {
                cout << "Sorry No Cars Available In Your Area." << endl;
                return false;
            }
            else {
                const char* q = query.c_str();
                qstate = mysql_query(conn, q);
                if (!qstate) {
                    res = mysql_store_result(conn);
                    cout<< setw(f_length) << "Car ID" <<setw(f_length) << "Car Brand" <<  setw(f_length) << "Car Name" << endl;
                    while (row = mysql_fetch_row(res)) {
                        cout <<setw(f_length) << row[0] << setw(f_length) << row[1] << setw(f_length) << row[2]<< endl;
                        Car_ids.push_back(row[0]);
                    }
                    cout << endl;
                }
                else {
                    cout << "Query to Get carinfo failed";
                    return false;
                }
            }
        }
        else {
            cout << "Query Failed";
            return false;
        }
        return true;
    }
    void Car_info(string car_id, string state) {
        int field_length = 25;
        int fl2 = 12;
        string q1 = "Select Car_info.Car_info_ID, Car_Brand, Car_Name, Car_Color, Car_Type, Car_Transmission, Car_Seats, Car_Year,car_price.price";
        string q2 = " From cars, car_info,car_price where cars.car_ID = car_info.car_id && car_info.car_state like '" + state + "' ";
        string q3 = "&& cars.car_id =" + car_id + " && car_price.car_info_id=car_info.car_info_id;";
        string query = q1 + q2 + q3;
        const char* q = query.c_str();
        qstate = mysql_query(conn, q);;
        if (!qstate) {
            res = mysql_store_result(conn);
            //Row[i] returns string.
            cout << setw(fl2) << "Car_info_ID" <<setw(fl2) << "Car Brand" << setw(field_length) << "Car Name"
                << setw(field_length);
            cout << "Car Color" << setw(fl2) << "Car Type" << setw(field_length) << "Car Transmission" << setw(fl2);
            cout << "Car Seats" << setw(fl2) << "Car Year" << setw(fl2) << "Car Price" << setw(fl2) << endl;
            while (row = mysql_fetch_row(res)) {

                cout << setw(fl2) << row[0] << setw(fl2) << row[1] << setw(field_length) << row[2] << setw(field_length)
                    << row[3]<<setw(fl2);
                cout <<setw(fl2) << row[4] << setw(fl2)<< setw(field_length) << row[5]<< setw(fl2) << row[6] << setw(fl2)
                    << row[7] << setw(fl2) << row[8] << setw(fl2) << endl;
            }
            cout << endl;
        }
        else cout << "Car info cant be Displayed.";
    }
    int validcarinfoid(string car_id, string state, string carinfoid) {
        int vaid_or_not = 0;

        string q1 = "Select Car_info.Car_info_ID, Car_Brand, Car_Name, Car_Color, Car_Type, Car_Transmission, Car_Seats, Car_Year,car_price.price";
        string q2 = " From cars, car_info,car_price where cars.car_ID = car_info.car_id && car_info.car_state like '" + state + "' ";
        string q3 = "&& cars.car_id =" + car_id + " && car_price.car_info_id=car_info.car_info_id && car_info.car_info_id=" + carinfoid + " ;";
        string query = q1 + q2 + q3;
        const char* q = query.c_str();
        qstate = mysql_query(conn, q);
        if (!qstate) {
            res = mysql_store_result(conn);
            row = mysql_fetch_row(res);
            if (row == NULL) {
                cout << "Car_Info_ID is not valid" << endl;
                vaid_or_not = 0;// Fail
            }
            else {
                vaid_or_not = 1;// Success
            }
        }
        return vaid_or_not;
    }
    double get_car_price(string car_info_id) {

        string semi = " ;";
        string q1 = " Select price From Car_info, Car_price where Car_info.Car_info_ID = Car_Price.Car_info_ID &&";
        string q2 = " car_price.car_info_ID =" + car_info_id + semi;
        string query = q1 + q2;
        const char* q = query.c_str();
        string price;
        double pric = 0.0;
        qstate = mysql_query(conn, q);;
        if (!qstate) {
            res = mysql_store_result(conn);
            while (row = mysql_fetch_row(res)) {
                price = row[0];
            }
            pric = stod(price);// Converts string to double.
        }
        else cout << "Could not Get Car Price";
        return pric;
    }
    void Purchasing_Rental(string UID, string Car_info_id, string Start_Date, string End_Date, string Card_num, string Card_exp) {
        // Gotta get price and cal difference in days
        double price =get_car_price(Car_info_id);
        double total_price = price * days;
        string t_price = to_string(total_price);
        string comma = ",";
        string q1 = "Insert into rentals(UID,Car_info_ID,Start_Date,End_date,Card_num,Card_Exp_Date,Total_Price) Values";
        string q2 = "(" + UID + comma + Car_info_id + comma + "'" + Start_Date + "'" + comma + "'" + End_Date + "'" + comma + "'" + Card_num + "'";
        string q3 = comma + "'" + Card_exp + "-05" + "'" + comma + t_price + ");";
        string query = q1 + q2 + q3;
        const char* q = query.c_str();
        qstate = mysql_query(conn, q);;
        if (!qstate) {
            res = mysql_store_result(conn);
        }
        else cout << "Query Failed";
    }
    bool Deleting_a_rental(string userid, string Start_date) {
        int vaid_or_not = 0;
        string query = "Delete from Rentals where UID =" + userid + " AND Start_Date ='" + Start_date + "' ;";
        const char* q = query.c_str();
        qstate = mysql_query(conn, q);
        if (!qstate) {
            res = mysql_store_result(conn);
            }
        else {
            cout << "Could Not Delete Rental" << endl;
            return false;
        }
        return true;
    }
    bool Viewing_Rental(string userid,string Date_Start) {//User can pick a car model.
        string q1 = "Select Rentals.Rental_ID, C_Users.UID,C_Users.First_Name,C_Users.Last_Name, Rentals.Start_Date, Rentals.End_Date, Cars.Car_ID,";
        string q2="Car_info.Car_Info_ID, Car_info.Car_State, Cars.Car_Brand, Cars.Car_name, Car_info.Car_Color, Car_info.Car_Type,";
        string q3 = "Car_Info.Car_Transmission, Car_info.Car_Seats, Car_info.Car_Year, Car_Price.Price, Rentals.Total_Price ";
        string q4="From C_users, Cars, Car_info, car_price, rentals Where Rentals.UID = C_Users.UID AND Cars.Car_ID = Car_info.Car_Id";
        string q5 =" AND Car_Price.Car_info_ID = Car_info.Car_info_ID AND Rentals.Car_info_ID = Car_Price.Car_Info_ID";
        string q6=" AND C_users.UID ="+userid+ " AND  Start_Date ='"+Date_Start+"'";
        string query = q1 + q2+q3+q4+q5+q6+";";
        string query2 = "SELECT EXISTS(" + q1 + q2+q3+q4+q5+q6 + ");";
        const char* q = query2.c_str();
        qstate = mysql_query(conn, q);
        if (!qstate) {
            res = mysql_store_result(conn);
            row = mysql_fetch_row(res);
            string test = row[0];
            if (test == "0") {
                cout << "Sorry There is Not rental with That Date" << endl;
                return false;
            }
            else {
                const char* q = query.c_str();
                qstate = mysql_query(conn, q);
                if (!qstate) {
                    res = mysql_store_result(conn);
                    while (row = mysql_fetch_row(res)) {
                        cout <<"\t"<< "Rental _ID:" << row[0] << endl;
                        cout << "\t" << "Name:" <<row[2]<<" "<<row[3]<< endl;
                        cout << "\t" << "Start Date:"<<row[4]<<" " << "End Date:"<<row[5]<<endl;
                        cout << "\t" << "State for Rental:"<<row[8] << endl;
                        cout << "\t" << "Car ID:"<<row[6] << "| Car Info ID:" <<row[7]<< endl;
                        cout << "\t" << "Car Brand:" << row[9] << "| Car Name:" << row[10] << "| Car Color:" << row[11] << endl;
                        cout << "\t" <<"Car Type:"<<row[12] <<"| Car Transmission:" << row[13] << "| Car Seats:" << row[14]
                            << "| Car Year:" << row[15]<<endl;
                        cout << "\t" << "Price Per Day:$" << row[16] << "| Total Price:$" << row[17] << endl;
                    }
                }
                else {
                    cout << "Query to Get carinfo failed";
                    return false;
                }
            }
        }
        else {
            cout << "Query Failed";
            return false;
        }
        return true;
    }
    bool Vaild_car_id(string carid) {
        bool valid = false;
       // string s_card_id = to_string(carid);
        for (int i = 0; i < Car_ids.size(); i++) {
            cout << "CarID " << Car_ids[i] << " ";
            if (Car_ids.at(i) == carid) {
                valid = true;
                break;
            }
            else valid = false;
        }
        cout << endl;
        return valid;
    }
    bool If_rental_exits(string userid, string Date_Start) {// Checking if rental with that userid and startdate exits if exits put info in a vector.
        string q1 = " Select Rental_Id,UID,Rentals.Car_info_ID,Start_Date,End_Date,Card_Num,Card_Exp_Date,Car_Price.price, Car_info.Car_State";
        string q2 = " From Rentals,Car_Price,Car_info";
        string q3 = " Where Rentals.Car_info_ID=Car_Price.Car_info_ID AND Car_info.Car_info_ID=Car_Price.Car_info_ID" ;
        string q4 = " AND Rentals.UID =" + userid + " AND  Start_Date ='" + Date_Start + "'";
        string query = q1 + q2 + q3 + q4 + ";";
        string query2 = "SELECT EXISTS(" + q1 + q2 + q3 + q4 +");";
        const char* q = query2.c_str();
        qstate = mysql_query(conn, q);
        if (!qstate) {
            res = mysql_store_result(conn);
            row = mysql_fetch_row(res);
            string test = row[0];
            if (test == "0") {
                cout << "Sorry There is Not rental with That Date" << endl;
                return false;
            }
            else {
                const char* q = query.c_str();
                qstate = mysql_query(conn, q);
                if (!qstate) {
                    res = mysql_store_result(conn);
                    while (row = mysql_fetch_row(res)) {
                        Rental_info_vec.push_back(row[0]);
                        Rental_info_vec.push_back(row[1]);
                        Rental_info_vec.push_back(row[2]);
                        Rental_info_vec.push_back(row[3]);
                        Rental_info_vec.push_back(row[4]);
                        Rental_info_vec.push_back(row[5]);
                        Rental_info_vec.push_back(row[6]);
                        Rental_info_vec.push_back(row[7]);
                        Rental_info_vec.push_back(row[8]);
                    }
                }
                else {
                    cout << "Query 2 Failed" << endl;
                    return false;
                }
            }
        }
        else {
            cout << "Query Failed for rental exits" << endl;
            return false;
        }
        return true;
    }
    bool Updating_Dates(string Start_date,string End_date) {
        int cprice = stoi(Rental_info_vec.at(7))*days;
        string scprice =to_string(cprice);
        string ren_id = Rental_info_vec.at(0);
        string q1 = "UPDATE Rentals SET Start_Date = '" + Start_date + "', End_Date = '" + End_date + "', Total_Price ="+scprice;
        string q2 = " WHERE Rental_ID=" + ren_id + " ;";
        string query = q1 + q2;
        const char* q = query.c_str();
        qstate = mysql_query(conn, q);
        if (!qstate) {
            res = mysql_store_result(conn);
        }
        else {
            return false;
            cout << "Sorry Could Not Update Rental Times" << endl;
        }
        return true;
    }
    bool Updating_Car(string carinfoid) {
        int cprice = get_car_price(carinfoid)*days;
        string scprice = to_string(cprice);
        string ren_id = Rental_info_vec.at(0);
        string q1 = "UPDATE Rentals SET Car_Info_ID =" +carinfoid+ ", Total_Price =" + scprice;
        string q2 = " WHERE Rental_ID=" + ren_id + " ;";
        string query = q1 + q2;
        const char* q = query.c_str();
        qstate = mysql_query(conn, q);
        if (!qstate) {
            res = mysql_store_result(conn);
        }
        else {
            return false;
            cout << "Sorry Could Not Update Car" << endl;
        }
        return true;
    }

    string get_state() {
        return Rental_info_vec.at(8);
     }
    string get_end_date() {
        return Rental_info_vec.at(4);
    }
    void Options() {
        cout << "Menu" << endl;
        cout << "1. Rent A Car" << endl;//Show Avaible cars with specs around location.
        cout << "2. View Your Rental" << endl;//View your rental
        cout << "3. Change Your Rental" << endl;
        cout << "4. Delete Your Rental" << endl;
        cout << "6. Exit" << endl;
        cout << "Choose One Option:";
    }
private:
    vector<string> Car_ids;
    vector<string> Rental_info_vec;
};
bool Vaild_state(string state) {
    string states[50] = { "AL","AK","AZ","AR","CA","CO","CT","DE","FL","GA","HI","ID","IL","IN","IA","KS","KY","LA","ME",
    "MD","MA","MI","MN","MS","MO","MT","NE","NV","NH","NJ","NM","NY","NC","ND","OH","OK","OR","PA","RI","SC","SD","TN",
    "TX","UT","VT","VA","WA","WV","WI","WY" };
    string Cap_state;
    bool test = false;
    if (state.length() > 2) return false;
    for (int i = 0; i < state.length(); i++) {
        char state_char = toupper(state[i]);
        Cap_state = Cap_state + state_char;
    }
    for (int i = 0; i < 50; i++) {
        if (states[i] == Cap_state) {
            test = true;
            break;
        }
        else test = false;
    }
    return test;
}
const int MAX_VALID_YR = 2060;
const int MIN_VALID_YR = 1980;
// Returns true if
// given year is valid.
bool isLeap(int year) {
    // Return true if year is a multiple pf 4 and not multiple of 100. OR year is multiple of 400.
    return (((year % 4 == 0) &&
        (year % 100 != 0)) ||
        (year % 400 == 0));
}
// Returns true if given // year is valid or not.
bool isValidDate(int d, int m, int y) {
    // If year, month and day  are not in given range
    if (y > MAX_VALID_YR || y < MIN_VALID_YR)return false;
    if (m < 1 || m > 12) return false;
    if (d < 1 || d > 31) return false;
    // Handle February month  with leap year
    if (m == 2) {
        if (isLeap(y)) return (d <= 29);
        else return (d <= 28);
    }
    // Months of April, June, Sept and Nov must have number of days less than or equal to 30.
    if (m == 4 || m == 6 || m == 9 || m == 11) return (d <= 30);
    return true;
}
bool Is_num(char num) {
    if (num == '0' || num == '1' || num == '2' || num == '3' || num == '4' || num == '5' || num == '6' || num == '7'
        || num == '8' || num == '9') return true;
    else return false;
}

void get_days(int start_day, int end_day) {
    days = abs(start_day - end_day);
}
int Is_valid_dates(string start_date, string end_date) {
    string s_year;
    string s_month;
    string s_day;
    string e_year;
    string e_month;
    string e_day;
    if ((start_date.length() > 10 || start_date.length() < 10) && (end_date.length() > 10 || end_date.length() < 10)) return 3;
    if (start_date.length() > 10 || start_date.length() < 10) return 1;
    if (end_date.length() > 10 || end_date.length() < 10) return 2;
    boolean is_num1;
    boolean is_num2;
    // Spliting String into year month and day.
    for (int i = 0; i < start_date.length(); i++) {
        if (i <= 3) s_year = s_year + start_date[i];
        else if (i >= 5 && i <= 6)  s_month = s_month + start_date[i];
        else if (i >= 8 && i <= 9)  s_day = s_day + start_date[i];

        if (i <= 3) e_year = e_year + end_date[i];
        else if (i >= 5 && i <= 6)  e_month = e_month + end_date[i];
        else if (i >= 8 && i <= 9)  e_day = e_day + end_date[i];
    }
    //Checking if strings has numbers only.
    string star_date;
    star_date = s_year + s_month + s_day;
    string e_date;
    e_date = e_year + e_month + e_day;
    for (int i = 0; i < star_date.length(); i++) {
        if ((Is_num(star_date[i]) == 0) && (Is_num(end_date[i]) == 0)) return 3;
        else if (Is_num(star_date[i] == 0)) return 1;
        else if (Is_num(e_date[i] == 0)) return 2;
    }
    //Converting strings to int so my function can find out if it is a valid date.
    int s1_day = stoi(s_day);
    int s1_month = stoi(s_month);
    int s1_year = stoi(s_year);

    int e1_day = stoi(e_day);
    int e1_month = stoi(e_month);
    int e1_year = stoi(e_year);
    if ((isValidDate(s1_day, s1_month, s1_year) == 0) && (isValidDate(e1_day, e1_month, e1_year) == 0)) return 3;
    if (isValidDate(s1_day, s1_month, s1_year) == 0) return 1;
    if (isValidDate(e1_day, e1_month, e1_year) == 0) return 2;
    get_days(s1_day, e1_day);
    //Return 1 means Start_date is bad. Return 2 means End Date is bad. Return 3 both dates are bad. Return 0 everything is good.
    return 0;
}
int Is_valid_start_date(string start_date) {// Used for deleting rentals or viewing rentals.
    string s_year;
    string s_month;
    string s_day;
    if (start_date.length() > 10 || start_date.length() < 10) return 1;
    // Spliting String into year month and day.
    for (int i = 0; i < start_date.length(); i++) {
        if (i <= 3) s_year = s_year + start_date[i];
        else if (i >= 5 && i <= 6)  s_month = s_month + start_date[i];
        else if (i >= 8 && i <= 9)  s_day = s_day + start_date[i];
    }
    //Checking if strings has numbers only.
    string star_date;
    star_date = s_year + s_month + s_day;
    for (int i = 0; i < star_date.length(); i++) {
        if (Is_num(star_date[i] == 0)) return 1;
    }
    //Converting strings to int so my function can find out if it is a valid date.
    int s1_day = stoi(s_day);
    int s1_month = stoi(s_month);
    int s1_year = stoi(s_year);
    if (isValidDate(s1_day, s1_month, s1_year) == 0) return 1;
    //Return 1 means Start_date is bad.
    return 0;
}
bool Valid_card_num(string card_num) {
    if (card_num.length() != 16) return false;
    for (int i = 0; i < 16; i++) {
        if (Is_num(card_num[i] == 0)) return false;
    }
    return true;
}
bool Valid_card_date(string card_date) {// Month-Year
    string c_year;
    string c_month;
    if (card_date.length() > 7 || card_date.length() < 7) return false;
    for (int i = 0; i < card_date.length(); i++) {
        if (i <= 3) c_year = c_year + card_date[i];
        else if (i >= 5) c_month = c_month + card_date[i];
    }
    string card_date2;
    card_date2 = c_year + c_month;
    for (int i = 0; i < card_date2.length(); i++) {
        if (Is_num(card_date2[i]) == 0) return false;
    }
    int c1_year = stoi(c_year);
    int c1_month = stoi(c_month);
    if (c1_year < 1980 || c1_year>2060 || c1_month > 12 || c1_month < 1) return false;
    return true;
}
void Options2() {
    cout << "-----------------Updating Info------------------" << endl << endl;
    cout << "Menu" << endl;
    cout << "1. Changing Dates" << endl;
    cout << "2. Changing Car" << endl;
    cout << "3. Changing State" << endl;
    cout << "Choose One Option:";
}
string sha256(const string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}
bool Email_pattern(string email) {
    const regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
    return regex_match(email, pattern);
}
string mask_pass() {
    char password[1000];
    char a;
    int i = 0;
    cout << "Enter Password:";
    while (1) {
        a = _getch();//Reads a single character from the console without echoing the character.
        if (a == 13) {//Asci Value for Enter.
            password[i] = NULL;
            break;
        }
        else if (a == 9) {//Asci Value for Tab
            continue;
        }
        else if (a == 8) {// Asci Value for backspace
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        }
        else {
            password[i] = a;
            i++;
            printf("*");
        }
    }
    return password;
}
int main() {
    int option;
    int option_for_mm;
    int option_for_um;
    bool test_for_acc = false;
    cout << "1. Login" << endl;
    cout << "2. Create A Account" << endl;
    cout << "Pick A Option:";
    int num;
    string state;
    string Car_id;
    string Car_info_id;
    string Start_date;
    string End_date;
    string Card_num;
    string Card_date;
    while ((!(cin >> option)) || (option != 1 && option != 2)) {
        cout << "Enter Option 1 OR Option 2 ";
        cin.clear();
        cin.ignore(123, '\n');
    }
    Database db;
    int s = db.connect();
    string email;
    string password;
    string F_name;
    string L_name;
    string pass_hash;

    if ((s == 1) && (option == 1)) {    // Then Login
        cout << "Enter Your Email:";
        cin >> email;
        cout << endl;
        password = mask_pass();
        cout << endl;
        pass_hash= sha256(password);
        int vaild_Acc = db.login(s, email, pass_hash);
        if (vaild_Acc == 0) {// If acc does not exit
            while (vaild_Acc != 1) {
                //cout<<
                cout << "Enter Your Email:";
                cin >> email;
                cout << endl;
                password = mask_pass();
                pass_hash = sha256(password);
                vaild_Acc = db.login(s, email, pass_hash);
            }
            goto Options_Label;
        }
        else {// If Account exits show options //Making sure user picks options 1-6 and is vaild.
        Options_Label:
            while ((!(cin >> option_for_mm)) || (option_for_mm != 1 && option_for_mm != 2 && option_for_mm != 3 && option_for_mm != 4
                && option_for_mm != 5 && option_for_mm != 6)) {
                cout << "Enter Option 1 OR Option 2 OR Option 3 OR Option 4 OR Option 5 OR Option 6:";
                cin.clear();
                cin.ignore(123, '\n');
            }
            if(option_for_mm == 6) return 0;// Exiting program
            else if (option_for_mm == 4) {// Deleting Rental.
                cout << endl;
                cout << "----------------------Deleting A Rental-------------------------"<<endl;
                Start_Label:// if Start_Date is not valid.
                cout << "Enter Your Start Date For your Rental:";
                cin >> Start_date;
                if (Is_valid_start_date(Start_date) == 0) {
                    if (db.Deleting_a_rental(user_id, Start_date) == 1) cout << "Rental Deleted" << endl;
                }
                else {
                    cout << "Not A Valid Date" << endl;
                    goto Start_Label;
                }
            }
            else if (option_for_mm == 3) {// Updating A Rental. Some options Change Car, Change State Change Dates
            Startdate1:
                cout << "Enter A Start Date:";
                cin >> Start_date;
                if (Is_valid_start_date(Start_date) == 0) {// valid
                    //Check db for rentals with that date
                    if (db.If_rental_exits(user_id, Start_date) == 1) {
                        Options2();
                        while ((!(cin >> option_for_um)) || (option_for_um != 1 && option_for_um != 2 && option_for_um != 3)) {
                            cout << "Enter Option 1 OR Option 2 OR Option 3:";
                            cin.clear();
                            cin.ignore(123, '\n');
                        }
                        if (option_for_um == 3) {// Changing State Only need state to view db of cars of that state rest is same as changing car.
                            cout << "Enter A State:";
                            cin >> state;
                            State_update_Label:
                            if (Vaild_state(state) == 1) {// Look for cars
                                if (db.Car_Models(state) == 1) {
                                Car_ID_Label2:
                                    cout << "Enter A Car_ID:";
                                    cin >> Car_id;
                                    if (db.Vaild_car_id(Car_id) == 1) {
                                        //Look into further cars editions.
                                        db.Car_info(Car_id, state); //Displays all info with car_info_id
                                    Car_info_ID_Label2:
                                        cout << "Enter Car Info_ID:";
                                        cin >> Car_info_id;
                                        if (db.validcarinfoid(Car_id, state, Car_info_id) == 1) {
                                            string e_date = db.get_end_date();
                                            string e1_date = e_date.substr(0, 10);
                                            if (Is_valid_dates(Start_date, e1_date) == 0) {
                                                if (db.Updating_Car(Car_info_id)) {//
                                                    cout << "Update Successful" << endl << endl;
                                                    cout << "--------------Rental Details----------------" << endl;
                                                    db.Viewing_Rental(user_id, Start_date);
                                                }
                                            }
                                        }
                                        else goto Car_info_ID_Label2;
                                    }
                                    else goto Car_ID_Label2;
                                }
                            }
                            else {
                                //Make user type a valid state
                                while (Vaild_state(state) != 1) {
                                    cout << "Enter A State:";
                                    cin >> state;
                                }
                                goto State_update_Label;
                            }
                        }
                        else if (option_for_um == 2) {//Changing Car
                            state = db.get_state();
                            if (Vaild_state(state) == 1) {// Look for cars
                                if (db.Car_Models(state) == 1) {
                                Car_ID_Label1:
                                    cout << "Enter A Car_ID:";
                                    cin >> Car_id;
                                    if (db.Vaild_car_id(Car_id) == 1) {
                                        //Look into further cars editions.
                                        db.Car_info(Car_id, state); //Displays all info with car_info_id
                                    Car_info_ID_Label1:
                                        cout << "Enter Car Info_ID:";
                                        cin >> Car_info_id;
                                        if (db.validcarinfoid(Car_id, state, Car_info_id) == 1) {
                                            string e_date = db.get_end_date();
                                            string e1_date = e_date.substr(0, 10);
                                            if (Is_valid_dates(Start_date, e1_date) == 0) {
                                                if (db.Updating_Car(Car_info_id)) {
                                                    cout << "Update Successful" << endl << endl;
                                                    cout << "--------------Rental Details----------------" << endl;
                                                    db.Viewing_Rental(user_id, Start_date);
                                                }
                                            }
                                        }
                                        else goto Car_info_ID_Label1;
                                    }
                                    else goto Car_ID_Label1;
                                }
                            }
                        }
                        else if (option_for_um == 1) {// Changing Dates
                        Both_Dates_Invaild_Label1:
                            cout << "Enter Starting Renting Date(YEAR-MONTH-DAY):";
                            cin >> Start_date;
                        End_Date_Invaild_Label1:
                            cout << "Enter Ending Renting Date(YEAR-MONTH-DAY):";
                            cin >> End_date;
                        Start_Date_Valid1:
                            if (Is_valid_dates(Start_date, End_date) == 0) {
                                if (db.Updating_Dates(Start_date, End_date) == 1) {
                                    cout << "Update Successful" << endl << endl;
                                    cout << "--------------Rental Details----------------" << endl;
                                    db.Viewing_Rental(user_id, Start_date);
                                }
                            }
                            else if (Is_valid_dates(Start_date, End_date) == 3) goto Both_Dates_Invaild_Label1;
                            else if (Is_valid_dates(Start_date, End_date) == 2) goto End_Date_Invaild_Label1;
                            else if (Is_valid_dates(Start_date, End_date) == 1) {
                                while (Is_valid_dates(Start_date, End_date) != 0) {
                                    cout << "Enter Starting Renting Date(YEAR-MONTH-DAY):";
                                    cin >> Start_date;
                                }
                                goto Start_Date_Valid1;
                            }
                        }
                    }
                }
                else goto Startdate1;
            }
            else if (option_for_mm == 2) {// Viewing Rental
                cout << "----------------------Viewing Rental----------------------" << endl;
            S_Label2:
                cout << "Enter Your Start Date:";
                cin >> Start_date;
                if (Is_valid_start_date(Start_date) == 0) {
                    db.Viewing_Rental(user_id, Start_date);
                    db.Options();
                    goto Options_Label;
                }
                else goto S_Label2;
            }
            else if (option_for_mm == 1) {// Option for renting a Car.
                cout << endl;
                cout << "Enter A State:";
                cin >> state;
                if (Vaild_state(state) == 1) {// Look for cars
                Car_State_Label:
                    if (db.Car_Models(state) == 1) {
                    Car_ID_Label:
                        cout << "Enter A Car_ID:";
                        cin >> Car_id;
                        if (db.Vaild_car_id(Car_id) == 1) {
                            //Look into further cars editions.
                            db.Car_info(Car_id, state); //Displays all info with car_info_id
                        Car_info_ID_Label:
                            cout << "Enter Car Info_ID:";
                            cin >> Car_info_id;
                            if (db.validcarinfoid(Car_id, state, Car_info_id) == 1) {
                            Both_Dates_Invaild_Label:
                                cout << "Enter Starting Renting Date(YEAR-MONTH-DAY):";
                                cin >> Start_date;
                            End_Date_Invaild_Label:
                                cout << "Enter Ending Renting Date(YEAR-MONTH-DAY):";
                                cin >> End_date;
                                if (Is_valid_dates(Start_date, End_date) == 0) { // Enter Credit Card info.
                                Dates_Label_Valid_Label:// Can be used if dates are vaild and if credit card num is not vaild.
                                    cout << "Enter Your Card Number:";
                                    cin >> Card_num;
                                    if (Valid_card_num(Card_num) == 1) {
                                    Card_date_Label:
                                        cout << "Enter Card Expiration Date(Year-Month):";
                                        cin >> Card_date;
                                        if (Valid_card_date(Card_date) == 1) {
                                            db.Purchasing_Rental(user_id, Car_info_id, Start_date, End_date, Card_num, Card_date);
                                            cout << "Success Rental Added"<<endl<<endl;
                                            db.Viewing_Rental(user_id, Start_date);
                                            db.Options();
                                            goto Options_Label;
                                        }
                                        else goto Card_date_Label;
                                    }
                                    else goto Dates_Label_Valid_Label;// If card_num is not valid.
                                }
                                else if (Is_valid_dates(Start_date, End_date) == 3) goto Both_Dates_Invaild_Label;
                                else if (Is_valid_dates(Start_date, End_date) == 2) goto End_Date_Invaild_Label;
                                else if (Is_valid_dates(Start_date, End_date) == 1) {
                                    while (Is_valid_dates(Start_date, End_date) != 0) {
                                        cout << "Enter Starting Renting Date(YEAR-MONTH-DAY):";
                                        cin >> Start_date;
                                    }
                                    goto Dates_Label_Valid_Label;
                                }
                            }
                            else goto Car_info_ID_Label;
                        }
                        else goto Car_ID_Label;
                    }
                }//ends here

                else { //Make user type a valid state
                    while (Vaild_state(state) != 1) {
                        cout << "Enter A State:";
                        cin >> state;
                    }
                    goto Car_State_Label;
                }
            }
        }
    }
    else if (s == 1 && (option == 2)) {
        cout << "--------------------Create A Account-----------------" << endl;
        email = "";
        password = "";
        cout << "Enter Your Email:";
        cin >> email;
        if (Email_pattern(email) == 0) {
            while (Email_pattern(email) == 0) {
                cout << "Not A Valid Email Enter Your Email Again:";
                cin >> email;
            }
        }
        cout << endl;
        password=mask_pass();
        pass_hash = sha256(password);
        cout << endl;
        cout << "Enter your First Name:";
        cin >> F_name;
        cout << "Enter your Last Name:";
        cin >> L_name;
        int acc_exits = db.Create_Account(email, pass_hash, F_name, L_name);
        if (acc_exits == 1) {
            while (acc_exits != 0) {
                cout << "Enter Your Email:";
                cin >> email;
                cout << endl;
                password = mask_pass();
                pass_hash = sha256(password);
                cout << endl;
                cout << "Enter your First Name:";
                cin >> F_name;
                cout << "Enter your Last Name:";
                cin >> L_name;
                acc_exits = db.Create_Account(email, pass_hash, F_name, L_name);
            }
        }
    }
    return 0;
}
