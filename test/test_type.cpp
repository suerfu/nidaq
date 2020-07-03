#include <iostream>
#include <cctype>
#include <vector>
#include <string>

using namespace std;

char GetType( vector<string> s );

char GetType( string s );


int main( int argc, char* argv[] ){

    string s;
    while( cin>>s ){
        cout << s << " is type " << GetType(s) << endl;
    }

    return 0;
}


char GetType( vector<string> s ){
    char ret = 'i';
    for( vector<string>::iterator itr = s.begin(); itr!=s.end(); itr++){
        char t = GetType( *itr );
        if( t=='s' ){
            ret = t;
            break;
        }
        else if( t=='f' ){
            ret = t;
        }
    }
    return ret;
}


char GetType( string s ){

    int dig_counter = 0;
        // digit counter
    int dec_counter = 0;
        // decimal point counter
    
    for(string::iterator itr = s.begin(); itr!=s.end(); itr++){

        // First if first character is minus sign, simply ignore it.
        if( itr==s.begin() && *itr=='-' )
            continue;

        // Iterate. If there is non-digit, non-decimal point, return
        else if( isdigit(*itr)==0 ){
            if( *itr!='.' )
                return 's';
            else
                dec_counter++;
        }

        else
            dig_counter++;
    }

    if( dig_counter>0  ){
        if( dec_counter==0 )
            return 'i';
        else if( dec_counter==1 )
            return 'f';
        else
            return 's';
    }
    else
        return 's';

}


