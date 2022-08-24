/***********************
   CS 202 Assignment 2

   Name: Keith Beauvais, 5005338658, CS 202 Assignment 2

   Description: This will read in the US Coast Guard database ship positions from
   the AIS. It will then ask users for a ship name and do a linear search through
   the vectors database and return matches with unique names. If the number returned
   was more than 0 it will prompt user to see if they want to calculate the distance
   that specific ship traveled based off of the haversine formula. There is a quit option
   built in as well.

   Input: The inputs are the AIS Data base as an infile, all other inputs are user inputs

   Output: Outputs are to the terminal and outputs are returning vessel information and distances.

***********************/

// Include libraries
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

// Structure to hold item data
struct AISType
{
    string MMSI;                // 0
    string baseDateTime;        // 1
    double lattitude;           // 2
    double longitude;           // 3
    double sog;                 // 4
    double cog;                 // 5
    double heading;             // 6
    string vesselName;          // 7
    string imo;                 // 8
    string callSign;            // 9
    string vesselType;          // 10
    string status;              // 11
    double length;              // 12
    double width;               // 13
    double draft;               // 14
    string cargo;               // 15
    string transceiverClass;    // 16
};
/*
class_identifier: AISClass
Contructors: AISClass();
            ~AISClass();
public functions: void readDataBase();
                  void newSearch( );
static members: None
*/
class AISClass{
    public:
    AISClass();
    ~AISClass();
    void readDataBase();
    void newSearch( );

    private:
    int searchForVesselByName ( string vesselName );
    void printRecord( int );
    string makeStringUpper( string s);
    bool getNextField(string &line, int &index, string &subString);
    void distanceTraveled( int first, int last );
    int findLastOccurrance(string mmsi );
    int findFirstOccurrance(string );
    double stringConvert( string);
    void addUniqueString( string value);
    void saveField( int fieldNumber, string subString,  AISType &tempItem );
    bool openInputFile(ifstream &);
    void readFile(ifstream &);
  // the dataBase
	// vector type is used because it's too big for an array.
	// (on my computer anyway)
	  vector<AISType> AISData;
    vector<string> uniqueMmsi;
    int recordCount;

};


int main()
{


    // declare an object of type AISClass
    AISClass Dbase;

    Dbase.readDataBase();
    Dbase.newSearch();

    return 0;
}

AISClass::AISClass() {
  recordCount = 0;
}


AISClass::~AISClass() {
  AISData.clear();
  uniqueMmsi.clear();
  cout << "Memory returned for vectors" << endl;
}


/*  distanceTraveled - finds the distance traveled from the ships starting location
to the ships ending location.

int first - this is the first found for either lattitude or longitude.

int last - this is the last found for either lattitude or longitude.

return value - void function does not return a value

  Algorithm
        Haversine formula was given in the assignment. First set R to be a const
        which is mean radius of earth in miles. Then set lattitude and longitude to
        radians and found the difference between both lattitudes and longitudes.
        Formulas previously given in assignment. It then prints the sitances and
        miles traveled.

*/
void AISClass::distanceTraveled(int first, int last ){


  double a, c, d;

  double R = 3958.8;
  //Putting lattitude and longitude in radians
  double lat1 = AISData[first].lattitude * 3.14159/180;
  double lat2 = AISData[last].lattitude * 3.14159/180;
  double long1 = AISData[first].longitude * 3.14159/180;
  double long2 = AISData[last].longitude * 3.14159/180;
  //
  double deltaLat = (lat2-lat1);
  double deltaLong = (long2-long1);

    a = pow(sin(deltaLat/2), 2) + cos(lat1)*cos(lat2)*pow(sin(deltaLong/2), 2);
    c = 2*atan2(sqrt(a), sqrt(1-a));
    d = R*c;

    //output the sitances and miles traveled
    cout << "Vessel: \"" << AISData[first].vesselName;
    cout << "\" MMSI: " << AISData[first].MMSI;
    cout << " Trip Starting time: " << AISData[first].baseDateTime;
    cout << endl;
    cout << "Distance Traveled from (" << AISData[first].lattitude;
    cout << ", " << AISData[first].longitude << ") to (";
    cout << AISData[last].lattitude << ", ";
    cout << AISData[last].longitude << ") ";
    cout << d;
    cout << " Miles" << endl;
    cout << endl;

    return;

}
    /* findLastOccurrance - finds the last occurrance of an entry in the dataBase
    using the MMSI as the matching criterion. The vector is searched from the
    last entry forward.  The vector  data is in time sequential order.

    string mmsi - the MMSI of the desired vessel.

    return value - the index of the last record in the dataBase for the vessel
    with the matching MMSI.  If the MMSI is not found, return -1.

    Algorithm
        Use a for loop the to seach from the last element of the vecotor toward
        the first. Since the data is time ordered, oldest first, search the
        vector from the bottom towards the top. This is a linear search and
        returns as soon as a match is found.

    */
int AISClass::findLastOccurrance(string mmsi){
    int found;

    for(int i = AISData.size()-1 ; i >= 0; i--){
    found = (AISData[i].MMSI.find(mmsi)!=string::npos);
      if(found){
        return i;
      }
    }

    return -1;
  }

    /* findFirstOccurrance - finds the first occurrance of an entry in the dataBase
    using the MMSI as the matching criterion. The vector is search from the
    first entry forward.  The vector data is in time sequential order.

    string mmsi - the MMSI of the desired vessel.

    return value - the index of the first record in the dataBase for the vessel
    with the matching MMSI.  If the MMSI is not found, return -1.

    Algorithm
        Used a for loop the to seach from the first element of the vecotor toward
        the Last. Since the data is time ordered, oldest first, searched from the
        vector from the top the bottom. This is a linear search and
        returns as soon as a match is found.

    */
int AISClass::findFirstOccurrance(string mmsi){

    int found;

    for(int i = 0; i < AISData.size(); i++){
    found = (AISData[i].MMSI.find(mmsi)!=string::npos);
      if(found){
        return i;
      }
    }
    return -1;
  }

    /* searchForVesselByName - searches for the vessel name that the user put in. From
    that search it records how many times that string/substring is found. Also calls
    addUniqueString in the function and passes in value vector<string> &s and

    string mmsi - the MMSI of the desired vessel.


    return value - the index of the first record in the dataBase for the vessel
    with the matching MMSI.  If the MMSI is not found, return -1.

    Algorithm:
        Used a for loop the to seach from the first element of the vecotor toward
        the Last. Since the data is time ordered, oldest first, searched from the
        vector from the top the bottom. This is a linear search and
        returns as soon as a match is found.

    */
int AISClass::searchForVesselByName(string vesselName){

    int found = 0;
    for(int i = 0; i < AISData.size(); i++){
      if(makeStringUpper(AISData[i].vesselName).find(vesselName)!=string::npos){
        AISClass::addUniqueString(AISData[i].MMSI);
        found++;
      }
    }
    return found;
  }

    /* addUniqueString - This function first searches the vector to determine if the
    string in already present. If the string is already present, the function simply
    returns. If the string is not present, the it is added at the end of the vector.

    string value - string to be added and also compared to the vector s

    return value - void function does not return a value.

    Algorithm:
        For loop to go through the vector s and and compare that to string value
        passed in. IF statement to say if it already present to return and if not
        the to add the string value passed in to the end of vector s.

    */
void AISClass::addUniqueString(string value){

    for(int i=0; i < uniqueMmsi.size(); i++){
      if(uniqueMmsi[i] == value){
        return;
      }
    }
    uniqueMmsi.push_back(value);
  }

    /* stringConvert - This function converts strings into doubles.

    string s - this string is passed in from the saveField function.

    return value - returns a double from the string when called in saveField function.

    Algorithm:
        Uses the string stream fuctionality of c++. Stores the the string into
        a string stream type varible and then stores string stream varible into a
        double value.
    */
double AISClass::stringConvert(string s){

    stringstream ss;
    double value;

    ss << s;
    ss >> value;

    return value;
  }


    /* printRecord - This function prints all the records to the terminal.

    int recordNumber - The index for where the AISType is what is being passed
    into the function.

    return value - void function does not return a value. Prints to the terminal.

    Algorithm:
        cout to the terminal the records. Calls on all the different structs.

    */
void AISClass::printRecord( int recordNumber ){

    cout<< "MMSI: "<< AISData[recordNumber].MMSI << endl;
    cout<< "Base Date Time: "<< AISData[recordNumber].baseDateTime << endl;
    cout<< "Lattitude: "<< AISData[recordNumber].lattitude << endl;
    cout<< "Longitude: "<< AISData[recordNumber].longitude << endl;
    cout<< "SOG: "<< AISData[recordNumber].sog << endl;
    cout<< "COG: "<< AISData[recordNumber].cog  << endl;
    cout<< "Heading: "<< AISData[recordNumber].heading << endl;
    cout<< "Vessel Name: "<< AISData[recordNumber].vesselName << endl;
    cout<< "imo: "<< AISData[recordNumber].imo << endl;
    cout<< "Call Sign: "<< AISData[recordNumber].callSign << endl;
    cout<< "Vessel Type: "<< AISData[recordNumber].vesselType << endl;
    cout<< "Status: "<< AISData[recordNumber].status << endl;
    cout<< "Length: "<< AISData[recordNumber].length << endl;
    cout<< "Width: "<< AISData[recordNumber].width << endl;
    cout<< "Draft: "<< AISData[recordNumber].draft<< endl;
    cout<< "Cargo: "<< AISData[recordNumber].cargo << endl;
    cout<< "Transceiver Class: "<< AISData[recordNumber].transceiverClass << endl;

  }

    /* openInputFile -  This function prompts the user for a .csv file to open. If the
    user enters a q || Q then the function returns false and exits out. If the file name
    does not exist or if the file cannot be opened then the user is reprompted until a
    file name can be read and opened. Once that happens then it will return a true statement.

    ifstream & inFile - file stream varible that is passed into the function.

    return value - If the user types in q||Q then returns false statement. IF the
    file is read and opened returns a true statement.

    Algorithm:
        Prompts user for a file name. If the user types in a q || Q then the function
        will return false. If the user types in a file that cannot be opened then it
        goes into a while loop until the user types in a file name that can be read and
        opened and once that happens then the statement returns true.

    */
bool AISClass::openInputFile( ifstream & inFile ){
    string fileName;

    cout<< "Enter input File Name/ (q-quit): ";
    cin>> fileName;

    inFile.open(fileName);

    while(!inFile.is_open()){
      if(fileName == "q"){
        return false;
      }
      else if(fileName == "Q"){
        return false;
      }
      inFile.clear();
      inFile.ignore(100, '\n');
      cout<< "Enter input File Name/ (q-quit): ";
      cin >> fileName;
      inFile.open(fileName);
    }
    return true ;
}

    /* readFile -  This function uses the previously opened filestream to read in the
    data from the file. Uses getline to read the data.  Once the line of data has
    been read, uses getNextField() to parse the line, one field at a time.  The initial
    call after the getline() should have the index set to zero, so it starts at the
    beginning of the string. getNextField() will update the index. The function starts
    with a locally declared and initialized AISType temporary record. As the line is
    read/parsed and the fields are parsed, set the corresponding value in the temporary
    record. Once parsing the line is complete, and the temporary record is filled,
    uses push_back( ) to add the temporary record to the AISType vector.

    ifstream & inFile - file stream varible that is passed into the function.

    return value - Void function does not have a return value.

    Algorithm:
    This function uses the previously opened filestream to read in the
    data from the file. Uses getline to read the data.  Once the line of data has
    been read, uses getNextField() to parse the line, one field at a time.  The initial
    call after the getline() should have the index set to zero, so it starts at the
    beginning of the string. getNextField() will update the index. The function starts
    with a locally declared and initialized AISType temporary record. As the line is
    read/parsed and the fields are parsed, set the corresponding value in the temporary
    record. Once parsing the line is complete, and the temporary record is filled,
    uses push_back( ) to add the temporary record to the AISType vector.

    */
void AISClass::readFile( ifstream & inFile){

    string line, subString;
    int index, fieldNumber;
    char eraseLine[]= {'\r',27,'[','1','K'};
    AISType tempItem;


    getline(inFile, line);
    line = line + '\n';
    while(!inFile.eof()){
      index = 0;
      fieldNumber = 0;
      for(int i = 0; i < 17; i++ ){
        getNextField(line, index, subString);
        saveField(fieldNumber, subString, tempItem);
        fieldNumber++;
      }
    AISData.push_back(tempItem);
    fieldNumber = 0;
    recordCount++;
    getline(inFile, line);
    line = line + '\n';
    }

    if((recordCount % 10000)==0){
    cout<< eraseLine << recordCount;
    cout.flush();
    }
}

    /* readFile -  This function saves the subString in to fieldNumber in the record
    passed, tempItem. The subString can be converted to a double, depending on the
    fieldNumber. This function uses stringConvert() to perform the conversion from
    string to double.

    int fieldNumber - the number of the field, starting at zero

    string subString- the value to be saved in the field.

    AISType &tempItem - he record to which the field will be added.

    return value - Void function does not have a return value.

    Algorithm:
        Takes the passed in field number in the switch statement and correspondes
        that to the case #. If the subString needs to get converted to a double it
        calls on stringConvert().

    */
void AISClass::saveField( int fieldNumber, string subString,  AISType &tempItem ){

    switch(fieldNumber)
    {
      case 0:
        tempItem.MMSI = subString;
        break;
      case 1:
        tempItem.baseDateTime = subString;
        break;
      case 2:
        tempItem.lattitude = stringConvert(subString);
        break;
      case 3:
        tempItem.longitude = stringConvert(subString);
        break;
      case 4:
        tempItem.sog = stringConvert(subString);
        break;
      case 5:
        tempItem.cog = stringConvert(subString);
        break;
      case 6:
        tempItem.heading = stringConvert(subString);
        break;
      case 7:
        tempItem.vesselName = subString;
        break;
      case 8:
        tempItem.imo = subString;
        break;
      case 9:
        tempItem.callSign = subString;
        break;
      case 10:
        tempItem.vesselType = subString;
        break;
      case 11:
        tempItem.status = subString;
        break;
      case 12:
        tempItem.length = stringConvert(subString);
        break;
      case 13:
        tempItem.width = stringConvert(subString);
        break;
      case 14:
        tempItem.draft = stringConvert(subString);
        break;
      case 15:
        tempItem.cargo = subString;
        break;
      case 16:
        tempItem.transceiverClass = subString;
        break;
    }
  }


    /* getNextField -  This function gets a line of data from getline in readFile(),
    it goes from coma to coma saving the sub strings until end of line. The string
    found, not including the comma, is saved in the subString. Commas are skipped.
    Parses one character at a time.

    string &line – the line of data read from the file that needs to be parsed

    int &index – the current starting position of the parsing. The first time this
    function is called for a new line, index should be set to zero. The function
    hould update the index beforereturning, so that on the next call it will look at
    the next field.

    string &subString – the parsed string

    return value - Returns a false statement if the length of the string passed in is
    less than or equal to the index number. Returns true if it does not.

    Algorithm:
    This function gets a line of data from getline in readFile(),
    it goes from coma to coma saving the sub strings until end of line. The string
    found, not including the comma, is saved in the subString. Commas are skipped.
    Parses one character at a time.

    */
bool AISClass::getNextField(string &line, int &index, string &subString){


    subString = "";


    while(line[index] != '\n' && line[index]!=','){
      subString += line[index];
      index++;
    }
    index++;
    if (line.length() <= index){
      return false;
    }
    else
      return true;
}



    /* makeStringUpper - This function converts the passed string to upper case and returns it.

    string s – the string to be converted to upper case.

    return value – upper case version of passed string.

    Algorithm:
        Goes through the string one letter at a time and makes it an upper case.

    */
string AISClass::makeStringUpper(string s){

    for(int i =0; i < s.length(); i++){
      s[i]=toupper(s[i]);
    }
      return s;
}


/* readDataBase - This function is a public member of the class AISClass. It
alls on openInputFile(). If the function returns true then readFile() is called
on. It lastly reads the number of records counted.

return value – Void function does not return a value.

Algorithm:
      Starts off by calling openInputFile(). That returns whether the file was
      able to open or not. It then takes that file and passes it into readFile().
      From there it prints the recordCount number.

*/
void AISClass::readDataBase(){
  // input file
  ifstream inFile;

  // temporary strings
  string temp;
  string ansYN;


  string stars="";


  // open the input file
  if (openInputFile( inFile ) )
         cout << "File opened correctly "<<endl;
  else{
      cout << "Error opening file"<< endl << "Existing...." << endl;

  }

  // read the entire file into the dataBase
  readFile( inFile);

  cout << recordCount << " records read "<<endl;

  cin.ignore( 40, '\n');

}

/* newSearch - This function is a public member of the class AISClass. This function
handles alot of the user interactions by prompting for vessel name and calling
searchForVesselByName(). This function also calls on printRecord() function and
calling for distanceTraveled().

return value – Void function does not return a value.

Algorithm:
          Prompts user for vessel name. Uses getline for the string the user input.
          Turns the user input string into an uppercase string and then calls on
          the searchForVesselByName(). If it comes back with a 0 or less
          it says vessel not found. else it will come back with how many matched
          that search and how many unique vessels. It will then prompt user with looking
          up distance traveled. If at anytime the user inputs q it will exit the
          program.

*/
void AISClass::newSearch( ){
  ifstream inFile;


  // temporary strings
  string temp;
  string ansYN;

  int found=0;
  string stars="";
  int first =0, last =0;

  // user interaction loop
  do{

      // prompt the user for the input to search for.  q to quit
      temp.clear();
      uniqueMmsi.clear();

      cout << "Enter vessel name: ";

      // read the user input.  getline is used so that spaces may be included
      // in the input
      getline(cin, temp, '\n');

      // check to see if the user wants to exit the program.
      // If not exiting, output the search string.
      if ( temp != "q" or temp == "Q" ){
          cout << endl<< "Searching for records with names containing \""
               << temp << "\"" << endl;
      }
      else{
        exit(EXIT_FAILURE);
      }

      // search for the number of items that contain the name/phrase
      // All names in the vessel dataBase are upper case, so make the search
      // string upper.  MMSI is built by the function and contains the vector
      // of unique vessels that contain the name searched for.
      found = searchForVesselByName(makeStringUpper(temp));

      // Let the user know if any ships were found with the name
      if( found <= 0) {
          cout << "Vessel \"" << temp << "\" not found" << endl;
          continue;
      }else{
          // output the results of the search
          cout << stars << endl;
          cout << found << " vessels found with name containing \"" << temp
               << "\", ";
          cout <<  "Unique vessels: " <<  uniqueMmsi.size()  << endl;
          cout << stars << endl;

          // ships were found, see if the user wants to display them
          cout << uniqueMmsi.size() << " vessels found. Would you like to see their"
              << " first records? [y/n] " ;
          cin >> ansYN;

          if (ansYN =="y" or ansYN == "Y"){

              // print all the first records for the ships found
              for (unsigned int i=0; i<uniqueMmsi.size(); i++){

                  // find the vessels using MMSI and print the records
                  int index = findFirstOccurrance( uniqueMmsi[i] );

                  // verify that a valid record was found, print the record
                  if ( index != -1)
                   printRecord( index);
              }


              // Ask user if they want to calculate the distance traveled for
              // the vessel.
              cout << "Would you like to find the distance traveled for a vessel?"
                      " [y/n] " ;
              cin >> ansYN;

              if (  ansYN == "y" or ansYN == "Y"){
                  cout <<  "MMSI for vessel: ";
                  cin >> temp;
                  cout << stars << endl;

                  // locate the index value of the first and last record
                  first = findFirstOccurrance( temp );
                  last = findLastOccurrance( temp );

                  distanceTraveled( first, last);


              }
          }
      }
      cin.ignore( 40, '\n');

  }  while ( true );
}
