#pragma once

#include <stdio.h>
#include <string.h>
#include "AudioTools/AudioLogger.h"

namespace audio_tools {

/**
 * @brief A simple  wrapper to provide string functions on char*.
 * If the underlying char* is a const we do not allow any updates; The ownership
 * of the chr* must be managed externally!
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class Str {
    public:
        Str(){}
        
        // Creates a Str for string constant
        Str(const char* chars){
            if (chars!=nullptr){
                int len = strlen(chars);
                set((char*)chars, len, len, true);
            } else {
                this->is_const = true;
                clear();
            }
        }

        // Creates a Str with the indicated buffer
        Str(char chars[], int maxlen, int len=0){
            set(chars,maxlen,len, false);
        }

        Str (const Str & ) = default;	
        Str (Str && ) = default;	            
        Str& operator = (const Str & ) = default;	
        Str& operator = (Str && ) = default;	

        // assigs a value
        virtual void set(const char *alt){
            if (alt==nullptr){
                this->len = 0;
            } else {
                int new_len = strlen(alt);
                grow(new_len);
                this->len = new_len;
                if (this->isConst()){
                    // if the Str is a const we replace the pointer
                    this->maxlen = this->len;
                    this->chars = (char*) alt;
                } else {
                    // if the Str is an external buffer we need to copy
                    strncpy(this->chars,alt,this->maxlen);
                    this->chars[len]=0;
                }
            }
        }

        // assigs from another Str value
        virtual void set(const Str &alt){
            grow(alt.len);
            this->len = alt.len;

            if (this->isConst()){
                // if the Str is a const we replace the pointer
                this->chars = alt.chars;
            } else {
                // if the Str is an external buffer we need to copy
                strncpy(this->chars,alt.chars,this->maxlen);
                this->chars[len]=0;
            }
        }

        virtual void set(const char c){
            clear();
            add(c);
        }

        virtual void set(int value){
            clear();
            add(value);
        }

        virtual void set(double value, int precision=2, int withd=0){
            clear();
            add(value);
        }

        virtual void swap(Str &str){
            char* cpy_chars = chars;;
            bool cpy_is_const = is_const;
            int cpy_len = len;
            int cpy_maxlen = maxlen;
            
            chars = str.chars;
            is_const = str.is_const;
            len = str.len;
            maxlen = str.maxlen;

            str.chars = cpy_chars;
            str.is_const = cpy_is_const;
            str.len = cpy_len;
            str.maxlen = cpy_maxlen;
        }
    

        // assigns a memory buffer
        virtual void set(char chars[], int maxlen, int len=0, bool isConst=false){
            this->chars = chars;
            this->maxlen = maxlen;
            this->len = len;
            this->is_const = isConst;
            if (len==0){
                this->chars[0] = 0;
            }
        }

        // adds a int value
        virtual void add(int value){
            if (!this->isConst()){
                grow(this->length()+11);
                sprintf(this->chars+len,"%d",value);
                len = strlen(chars);
            }
        }

        // adds a double value
        virtual void add(double value, int precision=2, int withd=0){
            if (!this->isConst()){
                grow(this->length()+20);
                floatToString(this->chars+len, value, precision, withd);    
                len = strlen(chars);
            }
        }
    
       // adds a string
       virtual void add(const char* append){
            if (!isConst() && append!=nullptr){
                int append_len = strlen(append);
                grow(this->length()+append_len+1);
                int n = (len+append_len)<maxlen-1 ? append_len : maxlen - len -1;
                strncat(chars, append, n);
                chars[len+n]=0;
                len = strlen(chars);
            }
        }
    
        // adds a character
        virtual void add(const char c){
            if (!isConst() && len<maxlen-1){
                grow(this->length()+1);
                chars[len] = c;
                chars[++len]=0;
            }            
        }

        // checks if the string starts with the indicated substring
        virtual bool startsWith(const char* str){
            if (str==nullptr) return false;
            int len = strlen(str);
            return strncmp(this->chars,str, len)==0;
        }

        // checks if the string ends with the indicated substring
        virtual bool endsWith(const char* str){
            if (str==nullptr) return false;
            int endlen = strlen(str);
            return strncmp(this->chars+(len-endlen),str, endlen)==0;
        }
    
        virtual bool matches(const char* match){
            int m_size = strlen(match);
            if (length() < m_size)
                return false;
            if (strncmp(this->chars,match,m_size-1)!=0)
                return false;
            if (match[m_size-1]=='*' || match[m_size-1]==this->chars[m_size-1] ){
                return true;
            }
            return false;
        }


        // provides the position of the the indicated character after the indicated start position
        virtual int indexOf(const char c, int start=0){
            for (int j=start;j<len;j++){
                if (c==chars[j]){
                    return j;
                }
            }
            return -1;
        }
    
        // checks if the string contains a substring
        virtual bool contains(const char *str){
            return indexOf(str)!=-1;
        }

        // provides the position of the the indicated substring after the indicated start position
        virtual int indexOf(const char *cont, int start=0){
            if (chars==nullptr || cont==nullptr) return -1;
            int contLen = strlen(cont);
            for (int j=start;j<len;j++){
                char* pt = chars+j;
                if (strncmp(pt, cont, contLen)==0){
                    return j;
                }
            }
            return -1;
        }

        // provides the position of the last occurrence of the indicated substring
        virtual int lastIndexOf(const char *cont){
            if (cont==nullptr) return -1;
            int contLen = strlen(cont);
            for (int j=(len-contLen);j>=0;j--){
                if (indexOf(cont, j)!=1){
                    return j;
                }
            }
            return -1;
        }
    
        // we can assign a const char*
        virtual void operator=(const char* str)  {
            set(str);
        }

        // we can assign a char*
        virtual void operator=(char* str)  {
            set(str);
        }

        // we can assign a char
        virtual void operator=(char c)  {
            set(c);
        }
  
        // we can assign a double
        virtual void operator=(double val)  {
            set(val);
        }

        // we can assign an int
        virtual void operator=(int value)  {
            set(value);
        }

        // shift characters to the right -> we just move the pointer
        virtual void operator<<(int n){
            if (isConst()){
                this->chars+=n;
                this->len-=n;
            } else {
                memmove(this->chars,this->chars+n,len+1);
            }
        }
    
        virtual char operator[](int index){
            return chars[index];
        }

        // adds a substring at the end of the string
        virtual void operator+=(const char* str)  {
            add(str);
        }

        // adds a int at the end of the string
        virtual void operator+=(int value)  {
            add(value);
        }

        // adds a double at the end of the string
        virtual void operator+=(double value)  {
            add(value);
        }

        // adds a character
        virtual void operator+=(const char value)  {
            add(value);
        }

        // checks if the indicated string is equal to the current string
        virtual bool operator==(const Str &alt) const {
            if (this->len != alt.len)
                return false;
            return strncmp(this->chars, alt.chars, this->len)==0;
        }

        // checks if the indicated string is equal to the current string
        virtual bool operator==(const char *alt) const {
            return strncmp(this->chars, alt, this->len)==0;
        }

        // checks if the indicated string is different from the current string
        virtual bool operator!=(const Str &alt) const {
            return strncmp(this->chars, alt.chars, this->len)!=0;
        }

        // checks if the indicated string is different from the current string
        virtual bool operator!=(const char *alt) const {
            return strncmp(this->chars, alt, this->len)!=0;
        }

        // provides the string value as const char* 
        virtual const char* c_str() {
            return chars;
        }

        // provides the current length (filled with characters) of the string - excluding the
        // terminating 0
        virtual int length() {
            return len;
        }
    
        // checks if the string is empty
        virtual bool isEmpty(){
            return len==0;
        }
    
        // provides the maximum length of the string
        virtual int maxLength() {
            return maxlen;
        }

        // rmoves the indicated substring from the string
        virtual void removeAll(const char* toRemove){
            if (!isConst() && chars!=nullptr){
                int removeLen = strlen(toRemove);
                while(true){
                    int pos = indexOf(toRemove);
                    if (pos==-1){
                        break;
                    }
                    memmove((void*) (chars+pos), (void*) (chars+pos+removeLen), len - (pos + removeLen)+1);
                    len -= removeLen;
                }
            }
        }

        // limits the length of the string (by adding a delimiting 0)
        virtual void setLength(int len, bool addZero=true){
            if (!isConst() && addZero){
                this->savedChar = chars[len];
                this->savedLen = len;
                this->len = len;
                chars[len]=0;
            }
        }

        // undo the last setLength call
        virtual void setLengthUndo() {
            if (savedLen>=0){
                chars[len] = savedChar;
                this->len = savedLen;
                savedLen = -1;
            }
        }

        // copies a substring into the current string
        virtual void substring(Str& from, int start, int end){
            if (end>start){
                int len = end-start;
                grow(len);
                if (this->chars!=nullptr) {
                    len = len < this->maxlen ? len : this->maxlen;
                    strncpy(this->chars, from.chars+start, len);
                    this->len = len;
                    this->chars[len]=0;
                }    
            }
        }

        // copies a substring into the current string
        virtual void substring(const char* from, int start, int end){
            if (end>start){
                int len = end-start;
                grow(len);
                if (this->chars!=nullptr) {
                    strncpy(this->chars, from+start, len);
                    this->chars[len]=0;
                    this->len = len;
                }
            }
        }
    
        /// remove leading and traling spaces
        virtual void trim(){
            rtrim();
            ltrim();
        }

        /// count number of indicated characters as position
        virtual int count(char c, int startPos) {
            for (int j=startPos;j<len;j++){
                if (chars[j]!=c){
                    return j;
                }
            }
            return 0;
        }

        // remove leading spaces
        virtual void ltrim(){
            int n = count(' ',0);
            *this << n;
        }    
        
        /// remove trailing spaces
        virtual void rtrim(){
            if (!isConst()){
                while(this->endsWith(" ")){
                    len--;
                    chars[len] = 0;
                }
            }    
        }

        // clears the string by setting the terminating 0 at the beginning
        virtual void clear() {
            if (chars!=nullptr && !isConst()){
                chars[0] = 0;
            }
            len = 0;
        }

        // checks if the string is on the heap
        virtual bool isOnHeap() {
            return false;
        }

        // checks if the string is a constant that must not be changed
        virtual bool isConst() {
            return is_const;
        }

        // inserts a substring into the string
        virtual void insert(int pos, const char* str){
            if (!isConst()){
                int insert_len = strlen(str);
                grow(this->length()+insert_len);
                int move_len = this->len - pos + 1;
                memmove(chars+pos+insert_len, chars+pos, move_len);
                strncpy(chars+pos, str, insert_len);
            }
        }

        virtual bool equalsIgnoreCase(const char* alt){
            if (len != strlen(alt)){
                return false;
            }
            for (int j=0;j<len;j++){
                if (tolower(chars[j]) != tolower(alt[j]))
                    return false;
            }
            return true;
        }

    protected:
        char* chars = nullptr;
        bool is_const;
        int len;
        int maxlen;
        int savedLen = -1;
        char savedChar;


        // only supported in subclasses
        virtual bool grow(int newMaxLen){
            return false;
        }

        static char* itoa(int n, char s[]) {
            int i, sign;
            if ((sign = n) < 0)  /* record sign */
                n = -n;          /* make n positive */
            i = 0;
            do {       /* generate digits in reverse order */
                s[i++] = n % 10 + '0';   /* get next digit */
            } while ((n /= 10) > 0);     /* delete it */
            if (sign < 0)
                s[i++] = '-';
            s[i] = '\0';
            reverse(s);
            return s;
        }  

        static void reverse(char s[]) {
            int i, j;
            char c;

            for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
                c = s[i];
                s[i] = s[j];
                s[j] = c;
            }
        }  

       static char * floatToString(char * outstr, double val, int precision, int widthp){
            char temp[16];
            int i;

            // compute the rounding factor and fractional multiplier
            double roundingFactor = 0.5;
            unsigned long mult = 1;
            for (i = 0; i < precision; i++)
            {
                roundingFactor /= 10.0;
                mult *= 10;
            }
            
            temp[0]='\0';
            outstr[0]='\0';

            if(val < 0.0){
                strcpy(outstr,"-\0");
                val = -val;
            }

            val += roundingFactor;

            strcat(outstr, itoa(int(val),temp));  //prints the int part
            if( precision > 0) {
                strcat(outstr, ".\0"); // print the decimal point
                unsigned long frac;
                unsigned long mult = 1;
                int padding = precision -1;
                while(precision--)
                    mult *=10;

                if(val >= 0)
                    frac = (val - int(val)) * mult;
                else
                    frac = (int(val)- val ) * mult;
                unsigned long frac1 = frac;

                while(frac1 /= 10)
                    padding--;

                while(padding--)
                    strcat(outstr,"0\0");

                strcat(outstr,itoa(frac,temp));
            }

            // generate space padding
            if ((widthp != 0)&&(widthp >= strlen(outstr))){
                int J=0;
                J = widthp - strlen(outstr);
                
                for (i=0; i< J; i++) {
                    temp[i] = ' ';
                }

                temp[i++] = '\0';
                strcat(temp,outstr);
                strcpy(outstr,temp);
            }
            
            return outstr;
        }
 

};


}

