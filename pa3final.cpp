#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <sstream>
#include <ctime>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////
//room struct
struct Room { 
  string building_code;
  int room_number;
  int max_students;
};

//course struct
struct Course {
  string prefix;
  int course_number;
  int num_lectures;
  int num_minutes;
};

//section_constraint struct
struct Section_Constraints{
  int days[7]; // see explanation below
  int earliest_start_time;
  int latest_end_time;
};

//section struct
struct Section {
  int section_id;
  int section_number;
  int num_students;
  Course course;
  Section_Constraints constraints;
};

/////new struct/////
struct Scheduled_Sections {
  int section_id;
  string prefix;
  int course_number;
  int startTime;
  int endTime;
  int days[4];
  Room room;
};

//variable initialization
  string buffer;
  Room *rooms;
  Course *courses;
  Section_Constraints *constraints;
  Section *sections;
  Scheduled_Sections* scheduledSections;

//global variables to keep track of count of members in the structures
  int roomCount = 0;
  int scheduledCount=0;
  int sectionCount=0;
  int courseCount = 0;
  int nextVar = 0;

  char filein[265];
  

//////////////////////////////////////////////////////////////////////////////////
int findSectionNumber(int sectionID) {
  //Given a section id, returns the corresponding section number
  for(int i=0;i<sectionCount;i++) {
    if((*(sections +i)).section_id == sectionID) //compares the pointer with the sectionID
      return (*(sections +i)).section_number; //returns pointer data of section_number
  }
}

//////////////////////////////////////////////////////////////////////////////////
int findSectionStudents(int sectionID) {
  //Given a section id, returns the corresponding number of students in the section
  for(int i=0;i<sectionCount;i++) {
    if((*(sections +i)).section_id == sectionID) //copares the pointer with the sectionID
      return (*(sections +i)).num_students; //returns pointer data of num_students
  }
}

//////////////////////////////////////////////////////////////////////////////////
string finddays(int days[]) {
  //sets the corresponding days in the string based on the values set in date array
  string strDays=""; //declare string
  
    if(days[0]==1) //four if statements for the index of day array and compare it to 1 for output
      strDays = "M";
    if(days[1]==1)
      strDays = strDays + "T";
    if(days[2]==1)
      strDays = strDays + "W";
    if(days[3]==1)
      strDays = strDays + "R";
  
  return strDays;
}

//////////////////////////////////////////////////////////////////////////////////
void printScheduleByClass() {
  //prints the schedule for different classes
  for(int j=0;j<courseCount;j++) { //first for loop prints the string of course prefix (printed once per class)
    stringstream ss;
    ss << (*(courses+j)).course_number;
    string c2 =  (*(courses+j)).prefix + " " + ss.str();
    cout << c2 +"\n"+"--------"+"\n";
    for(int i=0;i<scheduledCount;i++) {   
      stringstream ss1;
      ss1 << (*(scheduledSections+i)).course_number;
      string c = (*(scheduledSections+i)).prefix + " " + ss1.str();
      if(c2==c) { //compares the the prefix to see if they are the same course 
        int sid=(*(scheduledSections+i)).section_id;
        cout<<"Course ID: "<<sid<<endl;
        cout<<"Section #: "<< setfill('0') << setw(2) <<findSectionNumber(sid)<<endl;
        cout<<"# Students: "<<findSectionStudents(sid)<<endl;
        cout<<"Room: "<<(*(scheduledSections+i)).room.building_code<< " "<<(*(scheduledSections+i)).room.room_number<<endl;
        cout<<"Capacity: "<<(*(scheduledSections+i)).room.max_students<<endl;
        cout<<"Meeting Day/Time: "<<finddays((*(scheduledSections+i)).days)<< " "<< setfill('0') << setw(4) <<(*(scheduledSections+i)).startTime<< "-"<< setfill('0') << setw(4) <<(*(scheduledSections+i)).endTime<<endl<<endl;
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////
void printScheduleByClassroom() {
  //prints the schedule for different classroom
  for(int j=0;j<roomCount;j++) { //first for loop uses stringstream and prints room and capacity
    stringstream ss,sscap;
    ss << (*(rooms+j)).room_number;
    sscap << (*(rooms+j)).max_students;
    string c2 =  (*(rooms+j)).building_code + " " + ss.str() + ", CAPACITY=" + sscap.str();
    cout << c2 +"\n"+"-------"+"\n";
    for(int i=0;i<scheduledCount;i++) { 
      stringstream ss1,sscap1;
      ss1 << (*(scheduledSections+i)).room.room_number;
      sscap1 << (*(scheduledSections+i)).room.max_students;
      string c =  (*(scheduledSections+i)).room.building_code + " " + ss1.str() + ", CAPACITY=" + sscap1.str();
      if(c2==c) { //compares the room and capacity to see if they are the same classroom
        int sid=(*(scheduledSections+i)).section_id;
        cout<<finddays((*(scheduledSections+i)).days)<< " "<< setfill('0') << setw(4) <<(*(scheduledSections+i)).startTime<< "-"<< setfill('0') << setw(4) <<(*(scheduledSections+i)).endTime;
        cout<<" "<<(*(scheduledSections+i)).prefix<<" "<<(*(scheduledSections+i)).course_number<<" "<< setfill('0') << setw(2) <<findSectionNumber(sid)<< " "<<sid;
        cout<<", NUM_STUDENTS="<<findSectionStudents(sid)<<endl;
        
      }
    }
    cout<<endl;
  }
}

//////////////////////////////////////////////////////////////////////////////////
int fixTime(int constraintStartTime, int numMinutes) {
  //increments the constrainttime by the number of minutes 
  int minutes = (constraintStartTime%100) + (numMinutes%60); //initializing minutes and hours
  int hours =0;
  if(minutes>=60) { //converts minutes over 60 back to orginal count
    hours = minutes /60;
    minutes = minutes %60;
  }
   hours = hours + constraintStartTime/100 + numMinutes/60;
   if(hours>=24) //converts hours to military time with modulus 24
    hours=hours%24;
  return (hours*100)+minutes;
 }

//////////////////////////////////////////////////////////////////////////////////
bool roomsMatch(Room scheduledRoom, Room room) {
  //checks whether the room number for both scheduled room and the given room are the same
  if(scheduledRoom.room_number==room.room_number)
    return true;
  else
    return false;
}

//////////////////////////////////////////////////////////////////////////////////
bool daysMatch(int* scheduledDays, int* constraintDays) {
  //checks whether the days array for both scheduled room and given room are the same
  for(int i=0;i<4;i++)
  {
    if(scheduledDays[i]!=constraintDays[i])
      return false;
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////////////
void sortRooms() {
  //sorts the room based on the capacity in descending order
  for(int i=0;i<roomCount;i++) {
    for(int j=i+1;j<roomCount;j++) { //
      if(rooms[i].max_students<(*(rooms +j)).max_students) { //checks if outer struct componenent is less than max_students component for the inner for loop
        Room tempRoom = (*(rooms +i));
        (*(rooms +i)) = (*(rooms +j));
        (*(rooms +j)) = tempRoom;
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////
int timesOverlap( int scheduledStartTime, int scheduledEndTime,
       int constraintStartTime, int constraintEndTime, int numMinutes ) {
  //Looking for a start time and end time, for the current class, that does
  //not overlap the scheduled start and end time, that are provided
  //Returns -1 if we cannot find a time. Otherwise returns the schedulable start time
  bool scheduled = false; //initialize bool as false
  while (!scheduled) { //while true
    if ( ( constraintStartTime >= scheduledStartTime && 
   constraintStartTime <= scheduledEndTime ) ||
         ( fixTime(constraintStartTime, numMinutes) >= scheduledStartTime &&
     fixTime(constraintStartTime, numMinutes) <= scheduledEndTime ) ) {
      //Can't start the new section at this time.
      //Advance the start time by numMinutes plus 10 minutes and return.
      //We must now compare this new start time with all scheduled sections
      return fixTime( constraintStartTime, numMinutes+10 );
    } else {
      //This is a possible scheduable start time. We must use the new times for future
      //schedule checks
      break;
    }
  }

  return constraintStartTime;
}

//////////////////////////////////////////////////////////////////////////////////
bool scheduleAvailable( Section sectionToSchedule, int roomIndex ) {
  int* constraintDays = sectionToSchedule.constraints.days; // see explanation below 
  int constraintStartTime = sectionToSchedule.constraints.earliest_start_time; 
  int originalStartTime = constraintStartTime;
  int constraintEndTime = sectionToSchedule.constraints.latest_end_time;
  int courseLength = sectionToSchedule.course.num_minutes; 
  int currentScheduledCount = scheduledCount;
  Course course = sectionToSchedule.course;

  bool canSchedule = false; //initalize bool as false
  int scheduleRoomIndex = roomIndex;
  int actualStartTime = constraintStartTime;
  int k=0;

  while ( k < scheduledCount ) {
    if ( canSchedule) {
      //We can schedule, so exit the for loop
      break;
    }

    //Check if there is a conflict with an already scheduled section
    Scheduled_Sections scheduledSection = scheduledSections[k];
    int scheduledStartTime = scheduledSection.startTime;
    int scheduledEndTime = scheduledSection.endTime;
    int* scheduledDays = scheduledSection.days;
    Room scheduledRoom = scheduledSection.room;
    
    actualStartTime  = constraintStartTime;

    int possibleStartTime = constraintStartTime;

    while ( true ) {
      if ( scheduleRoomIndex < 0 ) {
  //Have tried all the rooms. This section cannot be scheduled
  canSchedule = false;
  break;
      }

      if ( roomsMatch( scheduledRoom, rooms[scheduleRoomIndex] ) ) {
  //The needed room matches the already scheduled room
  if ( daysMatch( scheduledDays, constraintDays ) ) {
    //Days match. Check the times. If the days don't match, no overlap.
    possibleStartTime = timesOverlap( scheduledStartTime, scheduledEndTime,
             actualStartTime, constraintEndTime, 
             courseLength );
    if ( possibleStartTime != actualStartTime ) {
      //The start time has changed, we must start over with
      //checking scheduled sections, using this new start tim,
      //only if the end time is within the constraintEndTime
      if ( fixTime( possibleStartTime, courseLength+10 ) >
     constraintEndTime ) {
        //This section cannot be scheduled in this room. Try the next
        //bigger room, and start with the first scheduled section
        possibleStartTime = originalStartTime;
        scheduleRoomIndex--;
        break;
      }

      //The new end time is within the constraint end time, so we 
      //exit the while loop, set k back to 0, and try the new times
      //against all the scheduled sections
      break;
    }

    if ( actualStartTime == -1 ) {
      //Cannot schedule in this room. Try the next biggest room
      actualStartTime = originalStartTime;
      scheduleRoomIndex--;
      continue;
    }

    break;
  }
      }

      //Current scheduled section does not overlap the needed section. Try the
      //next scheduled section
      break;
    } //end of while

    //Must increment k since we have a for loop now
    k++;

    if ( possibleStartTime != actualStartTime ) {
      //Must start over using this new start time
      canSchedule = false;
      constraintStartTime = possibleStartTime;
      k=0;
    }
  } //end of for

  if ( ( scheduledCount == 0 || scheduledCount == k ) && scheduleRoomIndex != -1 ) {
    //No scheduled sections, so we can schedule, OR we went through all the
    //sections and found a schedulable time
    canSchedule = true;
  }

  if ( canSchedule ) {
    //We can scheule this section. So do it.
    scheduledSections[scheduledCount].startTime = actualStartTime;
    scheduledSections[scheduledCount].section_id = sectionToSchedule.section_id;
    scheduledSections[scheduledCount].prefix = course.prefix;
    scheduledSections[scheduledCount].course_number = course.course_number;
    scheduledSections[scheduledCount].endTime = fixTime( actualStartTime,
               course.num_minutes );
    scheduledSections[scheduledCount].room = rooms[scheduleRoomIndex];

    for ( int i=0; i<4; i++ ) {
      //Copy the day values
      scheduledSections[scheduledCount].days[i] = constraintDays[i];
    }
    scheduledCount++;
  }
 
  return canSchedule;
}

//////////////////////////////////////////////////////////////////////////////////
bool scheduleSections() {
  //Schedule sections from largest room to smallest room - in section size
  //Schedule all classes that can only fit in the largest room, that hasn't
  //yet been scheduled (starting with the largest room). When that is done
  //go to the next largest room, etc. 
  //This function assumes the rooms array is sorted from largest room to
  //smallest room.
  //roomCount is the total number of rooms that exist

  for ( int i=0; i<roomCount-1; i++ ) {
    //Use roomCount-1, since we check two room size values at a time
    int bigRoomSize = rooms[i].max_students;
    int smallRoomSize = rooms[i+1].max_students;

    //We want to schedule all the rooms that have a size greater than
    //smallRoomSize and less than or equal to bigRoomSize
    for ( int j=0; j<sectionCount; j++ ) {

      if ( sections[j].num_students > smallRoomSize && sections[j].num_students <= bigRoomSize ) {
  
       //This is a room we are to schedule
       bool scheduled = scheduleAvailable( sections[j], i );
       if ( !scheduled ) {
          //Have at least one section that cannot be scheduled
          cout<<"have a room that can't be scheduled\n";
          return false;
       }
      }
      else if (sections[j].num_students < smallRoomSize && (i==roomCount-2)) {
          bool scheduled = scheduleAvailable( sections[j], i+1);
          if ( !scheduled ) {
          //Have at least one section that cannot be scheduled
          cout<<"have a room that can't be scheduled\n";
          return false;
       }
      }
    }
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////
int main()
{
  cout << "Welcome to the classroom assignment game!" << endl; 
  cout << "Enter the input file: "; //prompts the user for an input
  cin.get(filein, 265);
  ifstream ifile,ifile2;
  ifile2.open (filein); //open file
  if (ifile2.is_open()) {
    getline(ifile2, buffer);
    while (!ifile2.eof()) {
    
      getline(ifile2, buffer);
      if(buffer.find("--") == 0) { //find the -- and ignore and skip
       nextVar++;
       if(sectionCount>0)
        sectionCount=0;
      continue;
      }
     if(buffer.size()==1){
      continue;
     }
     if(nextVar==0)
        roomCount++;
      if(nextVar==1)
        courseCount++;
      if(nextVar==2)
        sectionCount++;
    }
  }
  ifile2.close();
  ifile.open (filein);
  
if (ifile.is_open()) {
  while (!ifile) {
  cout << "Please reformat the file and try again. " << endl; //output reformat needed to user
  cin.clear();
  cin.ignore(10000, '\n');
  cin.get(filein, 265);   
  ifile.open (filein);
  }
int nextVar=0;
getline(ifile, buffer);
//////first section of the input file/////////
rooms= new Room[roomCount]; //dybamic allocation for the arrays
sections = new Section[sectionCount];
courses = new Course[courseCount];
constraints = new Section_Constraints[sectionCount];
roomCount = courseCount = sectionCount =0;

  while (!ifile.eof()) {
    
 getline(ifile, buffer);
 if(buffer.find("--") == 0) { //find the -- and skip
    nextVar++;
    if(sectionCount>0)
        sectionCount=0;
    continue;
  }
  if(buffer.size()==1){
    continue;
  }
  stringstream ss;
  ss << buffer;
if(nextVar==0) {
  ss >> (*(rooms+roomCount)).building_code //stringstream for components of struct
  >> (*(rooms+roomCount)).room_number
  >> (*(rooms+roomCount)).max_students;
  roomCount++;
  if (ss.fail()) //break if stringstream fails
    break;
}

//////second section of the input file/////////   
  if(nextVar==1) {
    if(courseCount>0){
    ss >> (*(sections+sectionCount)).section_id >> (*(courses+courseCount)).prefix >>(*(courses+courseCount)).course_number >> (*(sections+sectionCount)).section_number>>(*(courses+courseCount)).num_minutes >> (*(courses+courseCount)).num_lectures >> (*(sections+sectionCount)).num_students;
    (*(sections+sectionCount)).course = *(courses+courseCount);
    if(((*(courses+courseCount)).course_number != (*(courses+(courseCount-1))).course_number) || ((*(courses+courseCount)).prefix != (*(courses+(courseCount-1))).prefix)) {
    courseCount ++;
    sectionCount ++;
  }else{
    sectionCount++;
  }
}
else {/*for the first line*/
   ss >> (*(sections+sectionCount)).section_id >> (*(courses+courseCount)).prefix >>(*(courses+courseCount)).course_number >> (*(sections+sectionCount)).section_number>>(*(courses+courseCount)).num_minutes >> (*(courses+courseCount)).num_lectures >> (*(sections+sectionCount)).num_students;
    (*(sections+sectionCount)).course = *(courses+courseCount);
    courseCount ++;
    sectionCount ++;
}
  
  if (ss.fail()) //break if stringstream fails
    break;
  }

/////////////third section of input ////////////////////////////////
  if(nextVar==2) {
    string days;
       int temp = 0;
        ss >> temp >> days >> (*(constraints+sectionCount)).earliest_start_time >> (*(constraints+sectionCount)).latest_end_time;
  int j=0;
  while(days[j]!='\0') { //outputs the day of the week by comparing day array index to 1
    if(days[j]=='M')
       (*(constraints+sectionCount)).days[0]=1;
    if(days[j]=='T')
       (*(constraints+sectionCount)).days[1]=1;
    if(days[j]=='W')
       (*(constraints+sectionCount)).days[2]=1;
    if(days[j]=='R')
       (*(constraints+sectionCount)).days[3]=1;
    j++;
  }

  if (ss.fail()) { //break if stringstream fails
        break;
  }
  (*(sections+sectionCount)).constraints =  *(constraints+sectionCount);
  sectionCount++;
  
  }
  
}

////////////////////////////
   ifile.close();  //close file after reading
  }
  else 
  {
    cout << "Unable to open file!" << endl; //else output unable to open file to user
  }
  

    //schedule the sections
    sortRooms();
    scheduledSections = new Scheduled_Sections[sectionCount];
    bool isScheduled = scheduleSections();
    if(scheduledCount>0) { //output the schedule by class and classroom if applicable
        cout<<"SCHEDULE BY CLASS"<<endl<<"-----------------"<<endl<<endl;
        printScheduleByClass();
        cout<<"SCHEDULE BY CLASSROOM"<<endl<<"---------------------"<<endl<<endl;
        printScheduleByClassroom();
    }
    else{
      //cannot schedule the section
      cout<<"Unfortunately, I was not able to find a valid assignment of sections to rooms based on the given constraints."<<endl;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
