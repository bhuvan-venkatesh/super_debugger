#include <stddef.h>
#include <stdio.h>

// *** Run with: gcc -g -std=c99 -w testCamel.c -o testCamel 


int strCompare(char* a, char* b) {
	while (*a) {
		if (*b) {
			if (!(*a == *b))
				return 0;
		}
		
		a++;
		b++;
	}
	
	if (*b == '\0')
		return 1;
	return 0;
}


int equal(int idx, char** out, char*** ans) {
	if (out == NULL)
		return 0;
		
	char** sol = ans[idx];
	
	int i = 0;
	
	while (sol[i]) {
		if (out[i] == NULL)
			return 0;

		if (!strCompare(sol[i], out[i])) 
			return 0;

		++i;
	}
	
	if (out[i])				// need to check in case their solution is not NULL terminated
		return 0;
		
	return 1;
}

int test_camelCaser (char ** (* camelCaser)(const char *))
{
    char * inputs[] = {
        "   C  o d  3 sm3l 1 can be 1gnored with 1NCREDIBLE use of air fRESHener. God objects are the new religion.",
        "... .  , .. FREak of...!.,.Nature",
        ".   . ..   .HelloWORLD.  Hello?World. Hell0?W 0r1 d. Too Many periods above and spaces too",
        "Four Score AND SEvEn YeaRs Ago? Who SaID ThAT. ME;   1nTeresting. StuFf Huh? Yeah    ",
        "1234. Trying num5bers. Hi M0M     !",
        "a^2 + b ^  2 = c^2. Pytha9 Theorem   . Imagine there is no heaven it is easy if you try",
        "3Xtreme 7est Cas3s 71m3.T00 many s p  a c 3s;   why s0 ha   r D you ask? 1 w1 l l t3 l   l you. 5133p 50rt i  s  8est. agreed  ", 
        "{{catala n Num8ers}.  ; !   ?  !    . Huh? What are Th0se?! HmmMM M m m m.",
        "e = mc^2 is a FaM0u5 F0rmu1a f0und3d by whO ?    ",
        "    .    ,    queen!   of England is",
        "I w 1 ll show you A place,   h1gh 0n the desert plain,  W h 3   re th 3 Str 33 ts have n  0 nam3.",
        "Have   you come her 3 for f   0 rg1ven355? hav e you co me to raise the dead?  ?",
        "  Have yo u come here to play Jesus? To t he lepers in your head.",
        ". Did I ask too much.More than a lot!you gave me nothingNow its all I got& We Are one But we  ARe not the same;", 
        "1ate 4 class today 6uys.",
        "8675309 Jenny Jenny. 8675309jenny 8675309jenny! 8675309Jenny!",
        NULL
    };
    
    // All these were checked with the reference
    
	char* ans0[] = {
		"cOD3Sm3l1CanBe1GnoredWith1NcredibleUseOfAirFreshener",
		"godObjectsAreTheNewReligion",
		NULL
	};

	char* ans1[] = {
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"freakOf",
		"",
		"",
		"",
		"",
		"",
		"",
		NULL
	};

	char* ans2[] = {
		"",
		"",
		"",
		"",
		"",
		"helloworld",
		"hello",
		"world",
		"hell0",
		"w0R1D",
		NULL
	};

	char* ans3[] = {
		"fourScoreAndSevenYearsAgo",
		"whoSaidThat",
		"me",
		"1nteresting",
		"stuffHuh",
		NULL
	};

	char* ans4[] = {
		"1234",
		"tryingNum5bers",
		"hiM0m",
		NULL
	};

	char* ans5[] = {
		"a",
		"2",
		"b",
		"2",
		"c",
		"2",
		"pytha9Theorem",
		NULL
	};

	char* ans6[] = {
		"3xtreme7EstCas3s71M3",
		"t00ManySPAC3S",
		"whyS0HaRDYouAsk",
		"1W1LLT3LLYou",
		"5133p50RtIS8Est",
		NULL
	};


	char* ans7[] = {
		"",
		"",
		"catalaNNum8ers",
		"",
		"",
		"",
		"",
		"",
		"",
		"huh",
		"whatAreTh0se",
		"",
		"hmmmmMMMM",
		NULL
	};

	char* ans8[] = {
		"e",
		"mc",
		"2IsAFam0u5F0rmu1aF0und3dByWho",
		NULL
	};

	char* ans9[] = {
		"",
		"",
		"queen",
		NULL
	};

	char* ans10[] = {
		"iW1LlShowYouAPlace",
		"h1gh0NTheDesertPlain",
		"wH3ReTh3Str33TsHaveN0Nam3",
		NULL
	};

	char* ans11[] = {
		"haveYouComeHer3ForF0Rg1ven355",
		"havEYouCoMeToRaiseTheDead",
		"",
		NULL
	};

	char* ans12[] = {
		"haveYoUComeHereToPlayJesus",
		"toTHeLepersInYourHead",
		NULL
	};

	char* ans13[] = {
		"",
		"didIAskTooMuch",
		"moreThanALot",
		"youGaveMeNothingnowItsAllIGot",
		"weAreOneButWeAreNotTheSame",
		NULL
	};

	char* ans14[] = {
		"1ate4ClassToday6Uys",
		NULL
	};

	char* ans15[] = {
		"8675309JennyJenny",
		"8675309jenny8675309Jenny",
		"8675309jenny",
		NULL
	};
	
	char*** ans = malloc(16 * sizeof(char*));
	
	ans[0] = ans0;
	ans[1] = ans1;
	ans[2] = ans2;
	ans[3] = ans3;
	ans[4] = ans4;
	ans[5] = ans5;
	ans[6] = ans6;
	ans[7] = ans7;
	ans[8] = ans8;
	ans[9] = ans9;
	ans[10] = ans10;
	ans[11] = ans11;
	ans[12] = ans12;
	ans[13] = ans13;
	ans[14] = ans14;
	ans[15] = ans15;
	
//---------------------------------------------------------------------------------------------	
//---------------------------------------------------------------------------------------------	

    char ** input = inputs;
    int i = 0;
    while(*input){
        char **output = camelCaser(*input);
        if (!equal(i, output, ans)) {       	
        	// free before returning 0
        	free(ans);
        	
        	char** orig = output;
        
		    while(*output) {
				free(*output);
				*output = NULL;	
				output++;
			}
		
			free(orig);
			orig = NULL;
        	
        	return 0;
    	}
    	
    	//printf("i equals %d\n", i);
    	++i;
        input++;
        
        char** orig = output;
        
        while(*output) {
    		free(*output);
    		*output = NULL;	
    		output++;
		}
		
    	free(orig);
    	orig = NULL;
    }
    
    free(ans);
    ans = NULL;
    
    char **output = camelCaser(NULL);
    if (output != NULL) {
    	char** orig = output;
        
	    while(*output) {
			free(*output);
			*output = NULL;	
			output++;
		}
	
		free(orig);
		orig = NULL;
			
    	return 0;
	}
    
    return 1;
}

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------

int findPunct(const char* input_str) {
	int len = strlen(input_str);
	int count = 0;
	
	for (int i = 0; i < len; ++i) {
		if (ispunct(input_str[i]))
			++count;
	}
	
	return count;
}

int getSpaces(const char* input_str, int count) {
	int spaces = 0;

	for (int i = 0; i < count; ++i) {
		if(isspace(input_str[i])) 
			++spaces;
	}
	
	return spaces;
}

int isCap(char c) {
	if (c >= 'A' && c <= 'Z')
		return 1;
	return 0;
}

int isNum(char c) {
	if (c >= '0' && c <= '9')
		return 1;
	return 0;
}

void parseStr(const char* input_str, char** ret, int count, int idx) {
	int i = 0;
	while (isspace(input_str[i])) 
		++i;
	
	int spaces = getSpaces(input_str, count);
	
	ret[idx] = (char*)malloc(count - spaces);				// *** error (need + 1)
	char* begin = ret[idx];
	
	while (!isspace(input_str[i]) && !ispunct(input_str[i])) {				// first line
		if (isCap(input_str[i]) && !isNum(input_str[i])) 
			*ret[idx] = (char)((input_str[i]) - 'A' + 'a');
		else *ret[idx] = input_str[i];
	
		ret[idx]++;
		++i;
	}

	++i;
	
	if (!ispunct(input_str[i])) {
		int prev = 1;
		for (; i < count; ++i) {			// maybe use isspace || isalpha
			if (prev && !isspace(input_str[i])) {			// make sure not more than one consecutive space
				if (!isCap(input_str[i]) && !isNum(input_str[i])) 
					*ret[idx] = (char)((input_str[i]) - 'a' + 'A');
				else *ret[idx] = input_str[i];
			
				ret[idx]++;
				prev = 0;
				
				if (isNum(input_str[i]))
					prev = 1;
			}
			else if (isCap(input_str[i]) && !isspace(input_str[i])) {
				*ret[idx] = (char)((input_str[i]) - 'A' + 'a');
				ret[idx]++;	
			}
			else if (!isCap(input_str[i]) && !isspace(input_str[i])) {
				*ret[idx] = input_str[i];
				ret[idx]++;	
			}
			else if (isspace(input_str[i])) {
				prev = 1;
			}
		}
	}
	
	*ret[idx] = '\0';				// end string
	ret[idx] = begin;
}

void getSentences(const char* input_str, char** ret) {
	char* temp = input_str;		
	char* temp2 = input_str;	
	
	int len = strlen(input_str);
	int count = 0;						// will be length
	int idx = 0;

	for (int i = 0; i < len; ++i) {	
		temp++;
		
		if (ispunct(input_str[i])) {
			parseStr(temp2, ret, i - count, idx);
			count = i + 1;
			temp2 = temp;
			++idx;
		}		
	}
	
	ret[idx] = NULL;
}

char **camel_caser(const char *input_str) {
	if (input_str == NULL)
		return NULL;
		
	int numPunct = findPunct(input_str);
	
	// 1 more for NULL at the end
	char** output_s = (char*)malloc(numPunct * sizeof(char*) + 1);		// *** error (need (num + 1))

	getSentences(input_str, output_s);
		
    return output_s;
}

int main() {
    if(test_camelCaser(&camel_caser)) 
        printf("SUCCESS\n");
    else printf("FAILED\n");
}
