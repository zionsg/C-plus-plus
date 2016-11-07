/* Auxiliary Functions & Classes for Borland C++ Builder 5.0 */
/* Done by Zion Ng (Last updated: 30 Dec 2002, 1800hrs) */

/*
   Order:
      Application->MessageBox shortcuts
      Database Functions
      String Conversion Functions
      Other String Functions
      Math/Numeric Functions
      File Functions
      POS Classes
*/

#include <Math.h>   //needed for "round"
#include <stdio.h>  //needed for "writeLog"
#include <time.h>   //needed for "writeLog"


/*** Application->MessageBox shortcuts ***/

//If you want a message dialog where you can control which button
//is the default button, eg. No as default, use
//MessageBox instead of Application->MessageBox
//Eg: MessageBox(this->Handle,"Save changes?","Settings",MB_YESNO+MB_DEFBUTTON2)

//this is more so for use in debugging - just a simple popup
//another alternative is BCB's own ShowMessage()
void popup(AnsiString msg)
{
   Application->MessageBox(msg.c_str(),"",MB_OK);
}

//this is a easier-to-use version of MessageBox
int msgBox(AnsiString msg, AnsiString title, int flags)
{
   return Application->MessageBox(msg.c_str(),title.c_str(),flags);
}



/*** Database Functions ***/

//simplifies the opening of a TDatabase
void openDB(TDatabase* myDB, AnsiString DatabaseName)
{
   //will have error if try setting connected DB to new database
   myDB->Connected = false;
   myDB->DatabaseName = DatabaseName;
   myDB->AliasName = DatabaseName;
   myDB->LoginPrompt = false;
   myDB->Exclusive = false;
   myDB->KeepConnection = true;
   myDB->Connected = true;
}

//simplifies the running of an SQL statement
void openQuery(TQuery* myQuery, AnsiString mySQL)
{
   myQuery->Close();
   myQuery->SQL->Clear();
   myQuery->SQL->Add(mySQL);
   myQuery->Open();
}

//for queries that do not return result sets
void execQuery(TQuery* myQuery, AnsiString mySQL)
{
   myQuery->Close();
   myQuery->SQL->Clear();
   myQuery->SQL->Add(mySQL);
   myQuery->ExecSQL();
}

//shortcut to retrieve value as AnsiString from a field in a TQuery 
AnsiString getField(TQuery* myQuery, AnsiString fieldName)
{
   return myQuery->FieldByName(fieldName)->AsString;
}

//shortcut to retrieve boolean value from a boolean field in a TQuery 
//can't use the above method as no way to convert AnsiString to boolean
bool getBoolean(TQuery* myQuery, AnsiString fieldName)
{
   return myQuery->FieldByName(fieldName)->AsBoolean;
}



/*** String Conversion Functions ***/
//A = AnsiString, C = char*, T = TStringList*
//Can't really use TStrings cos abstract class

//converts AnsiString to char*
char* AtoC(AnsiString mystr)
{
   //Example: char* c = AtoC(Edit1->Text);
   char* ans = new char[mystr.Length() + 1];
   strcpy(ans, mystr.c_str());
   return ans;
}

//converts TStringList* to char*
char* TtoC(TStringList* mystr)
{
   //Example: char* c = TtoC(Query1->SQL);
   return mystr->GetText();
}

//converts char* to AnsiString
AnsiString CtoA(char* mystr)
{
   //Example: char* c = "test"; Edit1->Text = CtoA(c);
   AnsiString* ans = new AnsiString(mystr);
   return *ans;
}

//converts TStringList* to AnsiString
AnsiString TtoA(TStringList* mystr)
{
   //Example: Edit1->Text = TtoA(Query1->SQL);
   AnsiString ans = mystr->Text;
   return ans.Trim();
}

//converts char* to TStringList*
TStringList* CtoT(char* mystr)
{
   //Example: Query1->SQL = CtoT("select *");
   TStringList* ans = new TStringList;
   ans->SetText(mystr);
   return ans;
}

//converts AnsiString to TStringList*
TStringList* AtoT(AnsiString mystr)
{
   //Example: Query1->SQL = AtoT(Edit1->Text);
   TStringList* ans = new TStringList;
   ans->Text = mystr;
   return ans;
}

//converts int to AnsiString - for convenience
AnsiString ItoA(int i)
{
   AnsiString tmp;
   return tmp.sprintf("%d",i);
}

//converts double to AnsiString - for convenience
AnsiString DtoA(double d, int nodecimals)
{
   AnsiString tmp;
   AnsiString myformat;

   myformat = "%0.";
   myformat += nodecimals;
   myformat += "f";
   return tmp.sprintf(myformat.c_str(),d);
}



/*** Other String Functions ***/

//finds the position of findstr in mystr beginning from index
int posOf(AnsiString findstr, AnsiString mystr, int index)
{
   for (int cnt=index; cnt<=mystr.Length(); cnt++)
   {
      if (mystr.SubString(cnt,findstr.Length())==findstr)
         return cnt;
   }

   //if reach here, means no match, therefore return 0
   return 0;
}

//replaces all occurrences of findstr in mystr with replacestr
AnsiString replaceStr(AnsiString mystr, AnsiString findstr, AnsiString replacestr)
{
   AnsiString ans="";

   for (int cnt=1; cnt<=mystr.Length(); cnt++)
   {
      if (mystr.SubString(cnt,findstr.Length())==findstr)
      {
         ans += replacestr;
         cnt += findstr.Length()-1;
      }
      else
         ans += mystr.SubString(cnt,1);
   }

   return ans;
}

//repeats mystr for cnt times
AnsiString repeatStr(AnsiString mystr, int cnt)
{
   AnsiString ans="";
   for (int i=1; i<=cnt; i++)
      ans += mystr;

   return ans;
}

//for padding of mystr with spaces according to linewidth
//for example, pad "hello" with right alignment to fit POS display of 20 chars per line
//supports left, right and center alignment/justifying
//if mystr is longer than linewidth, it is truncated
AnsiString pad(AnsiString alignment, AnsiString mystr, int linewidth)
{
   if (mystr.Length()>linewidth)
      return mystr.SubString(1,linewidth);

   alignment = alignment.LowerCase();

   if ((alignment=="left") || (alignment=="l"))
   {
      while (mystr.Length()<linewidth)
         mystr = mystr + " ";
   }
   else if ((alignment=="right") || (alignment=="r"))
   {
      while (mystr.Length()<linewidth)
         mystr = " " + mystr;
   }
   else if ((alignment=="center") || (alignment=="c"))
   {
      //in the case that the no. of spaces before and after
      //mystr are not the same, the left side gets the extra space
      if ((linewidth>mystr.Length()) && ((linewidth-mystr.Length())%2==1))
         mystr = " " + mystr;
         
      while (mystr.Length()<linewidth)
         mystr = " " + mystr + " ";
   }

   return mystr;
}

//Joins 2 strings and full-justify them according to linewidth
//eg.  ie. first string left-aligns, second string right-aligns
//If any of the strings leaves no space for the other string, 
//it is truncated to half the linewidth, so that ultimately, 
//both strings will still be shown

AnsiString join2Str(AnsiString str1, AnsiString str2, int linewidth)
{
   AnsiString ans;

   if (str1.Length()>linewidth-str2.Length())
      str1 = str1.SubString(1,linewidth/2);

   if (str2.Length()>linewidth-str1.Length())
      str2 = str2.SubString(1,linewidth/2);

   ans = str1;
   while (ans.Length()<linewidth-str2.Length())
      ans = ans + " ";
   ans = ans + str2;

   return ans;
}   

//Removes frontcut chars from the beginning of mystr and
//backcut chars from the end of mystr and returns as AnsiString
//Useful for extracting values from strings such as
//VIP from (VIP)
AnsiString getStr(AnsiString mystr, int frontcut, int backcut)
{
   //remember that 1st char in an AnsiString is index 1, not 0.
   return mystr.SubString(frontcut+1,mystr.Length()-frontcut-backcut);
}



/*** Math/Numeric Functions ***/

//Rounds off "d" to "nodecimals" of decimal places
double round(double d, int nodecimals)
{
   double factor = pow(10.0,(double)nodecimals);
   d = (floor((d*factor)+0.5))/factor;
   return d;
}

//Removes frontcut chars from the beginning of myval and
//backcut chars from the end of myval and returns as double.
//Useful for extracting values from strings such as
//4.00 from ($4.00)
double getVal(AnsiString myval, int frontcut, int backcut)
{
   //remember that 1st char in an AnsiString is index 1, not 0.
   return myval.SubString(frontcut+1,myval.Length()-frontcut-backcut).ToDouble();
}

//Removes cutcnt no. of chars from the beginning of myval
//and returns as double. Useful for getting values
//from currency, eg. 4.00 from $4.00
double getVal(AnsiString myval, int cutcnt)
{
   //remember that 1st char in an AnsiString is index 1, not 0.
   return myval.SubString(cutcnt+1,myval.Length()-cutcnt).ToDouble();
}

//Same as above, just that this cuts one char off only
//included for convenience cos most of the time only
//need to remove $ signs from in front
double getVal(AnsiString myval)
{
   return myval.SubString(2,myval.Length()-1).ToDouble();
}


//calculates various statistics from an array of
//floating point numbers
double calcStats(double numarray[], int size, AnsiString anstype)
{
   //numarray is the the array of numbers from which
   //the statistics is to be calculated, size is
   //the number of elements in the array (need to pass in
   //as only the pointer to the 1st element of numarray
   //is passed in - no way to find the no. of elements in
   //numarray from there)
   //anstype is the type of ans the user wants -
   //Mean, Medium, Lowest Mode, Highest Mode, Mean Mode

   double Mean, Medium, LowestMode, HighestMode, MeanMode;

   //first, must sort the array
   int cnt, smallest;
   double tmp;
   cnt=0;
   while (cnt<size)
   {
      smallest = cnt;
      for (int i=cnt+1; i<size; i++)
      {
         if (numarray[i]<numarray[smallest])
            smallest = i;
      }
      tmp = numarray[cnt];
      numarray[cnt] = numarray[smallest];
      numarray[smallest] = tmp;
      cnt++;
   } //end of sort

   //calculate Mean
   double sum = 0;
   for (int i=0; i<size; i++)
      sum += numarray[i];
   Mean = sum/size;

   //calculate Medium
   //if odd, just take the middle element
   //if even, take the 2 middle elements and calc average
   if (size % 2 == 1)
      Medium = numarray[size/2];
   else
   {
      Medium = (numarray[size/2-1] +
                numarray[size/2]) / 2.0;
   }

   //calculate Mode, ie. frequency.
   //need 2 arrays - numarray and an array to keep track
   //of the frequencies of the elements in numarray. After that,
   //need to sort the 2 arrays according to the frequencies in ascending order.
   //So if the lowest frequency would be freqarray[0] and the element in
   //numarray corresponding to it, ie. the lowest mode, would be
   //numarray[0]. Dun really have to worry abt duplicate elements in numarray,
   //it won't affect the results.

   int *freqarray = new int[size];
   int freqcnt;
   for (int i=0; i<size; i++)
   {
      if (i>0 && numarray[i]==numarray[i-1])  //ie. same element
      {
         freqarray[i] = freqarray[i-1];
      }
      else
      {
         freqcnt=1;
         for (int j=i+1; j<size; j++)
         {
            if (numarray[j]==numarray[i])
               freqcnt++;
         }
         freqarray[i] = freqcnt;
      }
   } //end for

   //sort freqarray. Note that when swapping the elements in freqarray,
   //the corresponding elements in numarray must be swapped as well
   //so that they can maintain their relationship
   int tmpint;
   cnt=0;
   while (cnt<size)
   {
      smallest = cnt;
      for (int i=cnt+1; i<size; i++)
      {
         if (freqarray[i]<freqarray[smallest])
            smallest = i;
      }
      tmpint = freqarray[cnt];
      freqarray[cnt] = freqarray[smallest];
      freqarray[smallest] = tmpint;

      tmp = numarray[cnt];
      numarray[cnt] = numarray[smallest];
      numarray[smallest] = tmp;
      cnt++;
   } //end of sort

   //calc Modes
   LowestMode  = numarray[0];
   HighestMode = numarray[size-1];
   MeanMode    = (LowestMode + HighestMode) / 2;

   if (anstype=="Mean") return Mean;
   if (anstype=="Medium") return Medium;
   if (anstype=="Lowest Mode") return LowestMode;
   if (anstype=="Highest Mode") return HighestMode;
   if (anstype=="Mean Mode") return MeanMode;

   return 0;
}



/*** File Functions ***/

//This function just logs the last login time. With each call,
//the current DateTime replaces the previous DateTime in the
//file
//not generic - modify as needed
bool logDT()
{
   int myfile;
   AnsiString filename = "lastlogin.txt";
   AnsiString mystr = Date();

   if (!FileExists(filename))
      myfile = FileCreate(filename);
   else
      myfile = FileOpen(filename, fmOpenWrite);
   if (myfile>=0)
   {
      FileWrite(myfile,mystr.c_str(),mystr.Length());
      FileClose(myfile);
      return true;
   }
   else
      return false;
}

//This function just reads in the last login DateTime from
//the log file
//not generic - modify as needed
AnsiString readLastLogin()
{
   int myfile,filelen;
   char* ans;
   AnsiString filename = "lastlogin.txt";
   AnsiString mystr="";

   myfile = FileOpen(filename, fmOpenRead);
   if (myfile>=0)
   {
      filelen=10;   //"dd/mm/yyyy" FileSeek(myfile,0,2);
      FileSeek(myfile,0,0);
      ans = new char[filelen+1];
      FileRead(myfile, ans, filelen);
      mystr = CtoA(ans);
      FileClose(myfile);
   }

   return mystr.Trim().SubString(1,10);
}

//This function appends a user string and the current DateTime
//to the end of a logfile - used to keep track of all logins
//and logouts
//not generic - modify as needed
bool writeLog(char* mystr, char* logtype)
{
   FILE* myfile;
   char* filename = "poslog.txt";
   char* mode     = "at";          //"at"=append in text mode
   char* currtime;

   time_t now;
   time(&now);
   currtime = ctime(&now);

   myfile = fopen(filename, mode);
   fprintf(myfile, "%s %s %s", mystr, logtype, currtime);

   fclose(myfile);
   return true;
}



/*** POS Classes ***/

/* base class for POS Displays  */
// the setup is more so for serial port devices due to baud rate setting
// virtual methods to be implemented by device-specific classes
class POSDisplay
{
   private:
      AnsiString myPort;   //communcations port display is connected to
      int linewidth;       //no. of chars per line
      int numlines;        //no. of lines in display
      HANDLE myComm;       //handle for display device        

   public:    
      POSDisplay();
      bool setPort(AnsiString port);             //set port of display
      AnsiString getPort();                      //get port of display
      bool closePort();                          //close port of display
      void setLineWidth(int width);              //set line width of display
      int getLineWidth();                        //get line width of display
      void setNumLines(int num);                 //set no. of char per line
      int getNumLines();                         //get no. of char per line
      bool sendCmd(char msg[], int msglen);      //send cmd to display(char[] format)
      bool display(AnsiString msg);              //send msg to display
      //device-specific functions
      virtual bool clear() = 0;                  //clear display
      virtual bool get() = 0;                    //transmit view on display to comp
      virtual bool showDemo() = 0;               //show display demo
      virtual bool saveDemo() = 0;               //save current display as demo
}; //end definition of POSDisplay

//begin code implementation of DSP800
POSDisplay::POSDisplay()
{
   myPort = "COM1";  //default port
   linewidth = 0;
   numlines = 0;
   setPort(myPort);
}

bool POSDisplay::setPort(AnsiString port)
{
   myPort = port;
   DCB dcbCommPort;

   //must close port first in case port has been opened previously
   closePort();

   myComm = CreateFile(myPort.c_str(),
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       0,
                       OPEN_EXISTING,
                       0,
                       0);

  //if can't open port, exit
  if (myComm == INVALID_HANDLE_VALUE)
     return false;

  // SET BAUD RATE, PARITY, WORD SIZE, AND STOP BITS.
  // NOTE THAT BuildCommDCB() DEFAULTS TO NO HANDSHAKING.
  dcbCommPort.DCBlength = sizeof(DCB);
  GetCommState(myComm, &dcbCommPort);
  BuildCommDCB("9600,N,8,1", &dcbCommPort);
  SetCommState(myComm, &dcbCommPort);

  return true;
}  

AnsiString POSDisplay::getPort()
{
   return myPort;
}

bool POSDisplay::closePort()
{
  bool flag = true;

  flag = flag && PurgeComm(myComm, PURGE_RXABORT);  //terminate all read
  flag = flag && PurgeComm(myComm, PURGE_RXCLEAR);  //clear input buffer
  flag = flag && PurgeComm(myComm, PURGE_TXABORT);  //terminate all write
  flag = flag && PurgeComm(myComm, PURGE_TXCLEAR);  //clear output buffer
  flag = flag && CloseHandle(myComm);               //close handle

  return flag;
}

void POSDisplay::setLineWidth(int width)
{
   linewidth = width;
}

int POSDisplay::getLineWidth()
{
   return linewidth;
}

void POSDisplay::setNumLines(int num)
{
   numlines = num;
}

int POSDisplay::getNumLines()
{
   return numlines;
}

bool POSDisplay::sendCmd(char msg[], int msglen)
{
  //for serial port devices, no need to check if device is
  //offline...even if send msg to offline device, will not hang,
  //unlike printer port devices

  DWORD tmpint;
  return WriteFile(myComm,msg,msglen,&tmpint,NULL);
}   

bool POSDisplay::display(AnsiString msg)
{
   bool flag;

   //clear display first
   flag = clear();

   //have to do this way cos will have error if use msg.Length() in WriteFile()
   flag = flag && sendCmd(msg.c_str(),msg.Length());
   return flag;
}
//end code implementation for POSDisplay



/* base class for POS Receipt Printers  */
// this is more so for printer port devices
// virtual methods to be implemented by device-specific classes
class POSPrinter
{
   private:
      AnsiString myPort;   //communcations port printer is connected to
      int linewidth;       //no. of chars per line
      HANDLE myComm;       //handle for printer device
      
   public:
      POSPrinter();
      bool setPort(AnsiString port);          //sets and opens port of printer
      AnsiString getPort();                   //get port of printer
      bool closePort();                       //close port of printer
      void setLineWidth(int width);           //set line width of printer
      int getLineWidth();                     //get line width of printer
      bool sendCmd(char msg[], int msglen);   //send cmd to printer(char[] format)
      bool print(AnsiString msg);             //send msg to printer
      bool println();                         //prints newline char
      bool println(AnsiString msg);           //send msg to printer appended by newline char
      bool formFeed();                        //Formfeed
      //device-specific
      virtual bool cutReceipt() = 0;          //cut the receipt (for printers
                                              //with auto cut)
      virtual bool ejectDrawer() = 0;         //eject cash drawer (usually
                                              //connected to printer)
}; //end definition of POSPrinter

//begin code implementation for POSPrinter
POSPrinter::POSPrinter()
{
   myPort = "LPT1";  //default port
   linewidth = 0;
   setPort(myPort);
}

bool POSPrinter::setPort(AnsiString port)
{
   myPort = port;

   //must close port first in case port has been opened previously
   closePort();

   myComm = CreateFile(myPort.c_str(),
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       0,
                       OPEN_EXISTING,
                       0,
                       0);

  //if can't open port, exit
  if (myComm == INVALID_HANDLE_VALUE)
     return false;

  return true;
}  

AnsiString POSPrinter::getPort()
{
   return myPort;
}

bool POSPrinter::closePort()
{
  bool flag = true;

  flag = flag && PurgeComm(myComm, PURGE_RXABORT);  //terminate all read
  flag = flag && PurgeComm(myComm, PURGE_RXCLEAR);  //clear input buffer
  flag = flag && PurgeComm(myComm, PURGE_TXABORT);  //terminate all write
  flag = flag && PurgeComm(myComm, PURGE_TXCLEAR);  //clear output buffer
  flag = flag && CloseHandle(myComm);               //close handle

  return flag;
}

void POSPrinter::setLineWidth(int width)
{
   linewidth = width;
}

int POSPrinter::getLineWidth()
{
   return linewidth;
}

bool POSPrinter::sendCmd(char msg[], int msglen)
{
  //send a neutral control character, in this case, NULL,
  //to test if printer is online. If try to send to
  //offline printer, will hang the program
  if (!TransmitCommChar(myComm,0x00))  //ie. printer offline
     return false;

  DWORD tmpint;
  return WriteFile(myComm,msg,msglen,&tmpint,NULL);
}   

bool POSPrinter::print(AnsiString msg)
{
   //have to do this way cos will have error if use msg.Length() in WriteFile()
   return sendCmd(msg.c_str(),msg.Length());
}

bool POSPrinter::println()
{
   return sendCmd("\n",1);
}

bool POSPrinter::println(AnsiString msg)
{
   msg += "\n";

   //have to do this way cos will have error if use msg.Length() in WriteFile()
   return sendCmd(msg.c_str(),msg.Length());
}

bool POSPrinter::formFeed()
{
   return sendCmd("\f",1);
}
//end code implementation for POSPrinter



//this integrates all the POS functions into one class
//and also keeps tracks of POS terminal info
class POS
{
   private:
      AnsiString myBranch, myTerminal;
      int headerno, footerno;
      AnsiString* myReceiptHeader;
      AnsiString* myReceiptFooter;
      double myCashFlow;

   public:
      POSDisplay* myDisplay; //put public so that can access functions straight
      POSPrinter* myPrinter; //put public so that can access functions straight

      POS() { myBranch=""; myTerminal=""; headerno=0; footerno=0;
              myReceiptHeader = new AnsiString[headerno];
              myReceiptFooter = new AnsiString[footerno];
              myCashFlow = 0; 
              myDisplay = NULL;
              myPrinter = NULL;
            };
      void setDisplay(POSDisplay* disp) { myDisplay = disp; };
      void setPrinter(POSPrinter* prn)  { myPrinter = prn;  };
      void setBranch(AnsiString branch) { myBranch=branch;  };
      AnsiString getBranch()                { return myBranch;     };
      void setTerminal(AnsiString terminal) { myTerminal=terminal; };
      AnsiString getTerminal()              { return myTerminal;   };
      void setHeaderNo(int h)    { headerno=h;
                                   myReceiptHeader = new AnsiString[headerno];
                                 };
      int getHeaderNo()          { return headerno;   };
      void setFooterNo(int f)    { footerno=f;
                                   myReceiptFooter = new AnsiString[footerno];
                                 };
      int getFooterNo()          { return footerno;   };
      void setCashFlow(double c) { myCashFlow=c;      };
      int getCashFlow()          { return myCashFlow; };
      bool printReceiptHeader()
         {
            bool flag=true;
            for (int i=0; i<headerno; i++)
            {
               flag = flag && myPrinter->print(pad("c",myReceiptHeader[i],
                                 myPrinter->getLineWidth()));
            }
            return flag;
         };
      bool printReceiptFooter()
         {
            bool flag=true;
            for (int i=0; i<footerno; i++)
            {
               flag = flag && myPrinter->print(pad("c",myReceiptFooter[i],
                                 myPrinter->getLineWidth()));
            }
            return flag;
         };
}; //end class POS



//implementation of POSDisplay for
//DSP800 (GIGA-TMS Inc, Taiwan)
class DSP800 : public POSDisplay
{
   public:
      DSP800();
      bool clear();
      bool get();
      bool showDemo();
      bool saveDemo();
}; //end class DSP800 definition

//begin code implementation of DSP800
DSP800::DSP800() 
{  
   setLineWidth(20);
   setNumLines(2);
}

bool DSP800::clear()
{
  char clearCmd[6];
  clearCmd[0] = 0x04;   //EOT
  clearCmd[1] = 0x01;   //SOH - Start of Header
  clearCmd[2] = 0x43;   //COMMAND C - command for clearing whole POS display
  clearCmd[3] = 0x31;   //1st parameter - start pos
  clearCmd[4] = 0x58;   //2nd parameter - end pos
  clearCmd[5] = 0x17;   //ETB - End of Transmission Block

  return sendCmd(clearCmd,6);
}

bool DSP800::get()
{
   char transmitCmd[4];
   transmitCmd[0] = 0x04;  //EOT
   transmitCmd[1] = 0x01;  //SOH - Start of Header
   transmitCmd[2] = 0x54;  //COMMAND T - for transmitting current view to computer
   transmitCmd[3] = 0x17;  //ETB

   return sendCmd(transmitCmd,4);
}

bool DSP800::showDemo()
{
   char demoCmd[6];
   demoCmd[0] = 0x04;  //EOT
   demoCmd[1] = 0x01;  //SOH - Start of Header
   demoCmd[2] = 0x44;  //COMMAND D - command for displaying saved demo
   demoCmd[3] = 0x31;  //Choose Demo layer 1
   demoCmd[4] = 0x37;  //Display mode - horz + vertical + blinking
   demoCmd[5] = 0x17;  //ETB

   return sendCmd(demoCmd,6);
}

bool DSP800::saveDemo()
{
   char demoCmd[5];
   demoCmd[0] = 0x04;  //EOT
   demoCmd[1] = 0x01;  //SOH - Start of Header
   demoCmd[2] = 0x53;  //COMMAND S - command for saving current view msg as demo
   demoCmd[3] = 0x31;  //Save as Layer 1
   demoCmd[4] = 0x17;  //ETB

   return sendCmd(demoCmd,5);
}
//end code implementation of DSP800



//implementation of POSPrinter for
//Citizen Dot-Matrix iDP3551
class iDP3551 : public POSPrinter
{
   public:
      iDP3551();
      bool cutReceipt();
      bool ejectDrawer();

}; //end class iDP3551 definition

//begin code implementation of iDP3551
iDP3551::iDP3551() 
{  
   setLineWidth(40);
}

bool iDP3551::cutReceipt()
{
   char cutCmd[3];       //command for partial cut
   cutCmd[0] = 0x1B;
   cutCmd[1] = 0x50;
   cutCmd[2] = 0x01;

   bool flag;
   //must shift paper up some distance before cutting
   flag = sendCmd("\n\n\n\n\n\n\n\n\n\n",10);  
   flag = flag && sendCmd(cutCmd,3);

   return flag;
}

bool iDP3551::ejectDrawer()
{
   char ejectCmd[1];    //command for ejecting cash drawer 1
   ejectCmd[0] = 0x07;  

   return sendCmd(ejectCmd,1);
}
//end code implementation for iDP3551



/** END **/
