/* Benchparse.h contains the data types, structures, and constants
   for the C-language parser of the standard placement program input
   format specified by Preas and Roberts.


   Copyright (C) 1987 by Jonathan Rose.  All rights reserved.

*/


/* The parser is used by calling the procedure

	GetModule(InputFile,
		  Name,
		  Type, 
		  Width, 
		  Height, 
		  IOListHead, 
		  NetworkListHead, 
		  PlacementListHead); 

        FILE *InputFile;   is an already-opened file where the circuit is 
	NameType Name;
	ModuleType *Type;
	Number *Width;
	Number *Height;
	IOList **IOListHead;  pointer to a linked list of IOList entries
	NetworkList **NetworkListHead;  linked list of Network entries
	PlacementList **PlacementListHead; linked list of Placement entries

*/
/* Definition of Name Type  */

/* Arbitrarilty Chosen Maximum Length of a Name */

#ifndef __BENCHTYPES_H__
#define __BENCHTYPES_H__


#define MAXNAMESIZE 50

typedef char NameType[MAXNAMESIZE];


/* Definition of Module Type  */

#define STANDARD    1
#define PAD         2
#define GENERAL     3
#define PARENT      4
#define FEEDTHROUGH 5
#define ENDFILE     6

typedef short ModuleType;


/* Definition of "Number" Type  */

#define NOPOSITION -1.0
#define NOWIDTH    -1.0

typedef float Number;


/* Definition of Terminal Type, and Constants  */

#define I  1
#define O  2
#define B  3
#define PI 4
#define PO 5
#define PB 6
#define F  7

typedef short TerminalType;


/* Definition of Side Type and Constants  */

#define BOTTOM 1
#define RIGHT  2
#define TOP    3
#define LEFT   4
#define NOSIDE 5

typedef short SideType;


/* Definition of Layer Type and Constants  */

#define PDIFF  1
#define NDIFF  2
#define POLY   3
#define METAL1 4
#define METAL2 5
#define NOLAYER 6

typedef short LayerType;


/* Definition of the IOList Element and NULL Pointer  */

#define IOListNull 0

typedef struct IOListType {
    NameType SignalName;
    TerminalType Terminal;
    SideType Side;
    Number Position;
    Number Width;
    LayerType Layer;
    struct IOListType *Link;
} IOList;


/* Definition of the Signal List Element  and NULL Pointer */

#define SignalListNull 0

typedef struct SignalListType {
    NameType SignalName;
    struct SignalListType *Link;
} SignalList;


/* Definition of the Network List Element and NULL Pointer  */

#define NetworkListNull 0

typedef struct NetworkListType {
    NameType InstanceName;
    NameType ModuleName;
    SignalList *SignalListHead;
    struct NetworkListType *Link;
} NetworkList;


/* Definition of Reflection Type and Constants */

#define RFLNONE 1
#define RFLY    2

typedef short ReflectionType;


/* Definition of Rotation Type and Constants */

#define ROT0    1
#define ROT90   2
#define ROT180  3
#define ROT270  4

typedef short RotationType;


/* Definition of the Placement List Element and NULL Pointer */

#define PlacementListNull 0

typedef struct PlacementListType {
    NameType InstanceName;
    Number XLocation;
    Number YLocation;
    ReflectionType Reflection;
    RotationType Rotation;
    struct PlacementListType *Link;
} PlacementList;



/* Character strings that define the key words and comments */

/* Comment Delimiters */

#define STARTCOMMENT "/*"
#define ENDCOMMENT "*/"

/* End of entry delimeter */

#define LineTerminator ';'

/* Module Parameters */

#define ModuleKeyword       "MODULE"
#define EndModuleKeyword    "ENDMODULE"
#define TypeKeyword         "TYPE"
#define WidthKeyword        "WIDTH"
#define HeightKeyword       "HEIGHT"

/* IOList Delimiters */
#define IOListKeyword       "IOLIST"
#define EndIOListKeyword    "ENDIOLIST"

/* Network Delimiters */
#define NetworkKeyword      "NETWORK"
#define EndNetworkKeyword   "ENDNETWORK"

/* Placement Delimiters */
#define PlacementKeyword    "PLACEMENT"
#define EndPlacementKeyword "ENDPLACEMENT"

/* Module Types */
#define StandardKeyword     "STANDARD"
#define PadKeyword          "PAD"
#define GeneralKeyword      "GENERAL"
#define ParentKeyword       "PARENT"
#define FeedthroughKeyword  "FEEDTHROUGH"

/* IOList Terminal Types */
#define InputTerminal       "I"
#define OutputTerminal      "O"
#define BiDirectionTerminal "B"
#define PadInputTerminal    "PI"
#define PadOutputTerminal   "PO"
#define PadBiTerminal       "PB"
#define FeedThroughTerminal "F"

/* Side Types */
#define BottomSide "BOTTOM"
#define RightSide  "RIGHT"
#define TopSide    "TOP"
#define LeftSide   "LEFT"

/* Layer Types */
#define PDiffLayer   "PDIFF"
#define NDiffLayer   "NDIFF"
#define PolyLayer    "POLY"
#define Metal1Layer  "METAL1"
#define Metal2Layer  "METAL2"

/* Reflections */
#define NoReflection "RFLNONE"
#define YReflection  "RFLY"

/* Rotations */
#define Rot0   "ROT0"
#define Rot90  "ROT90"
#define Rot180 "ROT180"
#define Rot270 "ROT270"

#ifdef __cplusplus
extern "C" {
#endif

    void GetModule(FILE * TInputFile,
		   NameType Name,
		   ModuleType * ModType,
		   Number * Width,
		   Number * Height,
		   IOList ** IOListHead,
		   NetworkList ** NetworkListHead,
		   PlacementList ** PlacementListHead);


/* Definition of Module List */
/* Edited by Jinhwan Jeon 1995.2.13 */
    typedef struct ModuleListType {
	NameType Name;
	ModuleType ModType;
	Number Width;
	Number Height;
	IOList *IOListHead;
	NetworkList *NetworkListHead;
	PlacementList *PlacementListHead;
	struct ModuleListType *Link;
    } ModuleList;


#ifdef __cplusplus
}
#endif
#endif				// __BENCHTYPES_H__
