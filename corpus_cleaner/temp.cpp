#include <regex>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <vector>
#include <unistd.h>
#include <string>
#include <iostream>
#include <filesystem>
using namespace std;


//uint16の絵文字の場合は\34x
//uint32の絵文字の場合は\36x
string CalculateEmoji(string pre_emoji)
{
    string emoji=pre_emoji;
    //pre_emoji[0]='\360'の場合、pre_emojiは4要素でpre_emoji[1]='\237'で確定
    if(pre_emoji[0]=='\360'&&pre_emoji.size()!=4) return "";
    //pre_emoji[0]='\342'の場合、pre_emojiは3要素で確定
    if(pre_emoji[0]=='\342'&&pre_emoji.size()!=3) return "";

    emoji[emoji.size()-1]++;
    if(int(emoji[emoji.size()-1])>=-64){
        emoji[emoji.size()-1]=-128;
        emoji[emoji.size()-2]++;
    }
    return emoji;
}

void loop()
{

    //emoji
    string emoji = "🌀";
    //  129535-127744=1791
    for(int i=0;i<1792;i++){
        cout << emoji<<",";
        emoji=CalculateEmoji(emoji);
        if((i+1)%8==0)cout <<endl;
    }
    cout << endl;

    //special token
    emoji = "☀";
    //10239-9728=511
    for(int i=0;i<512;i++){
        cout << emoji<<",";
        emoji=CalculateEmoji(emoji);
        if((i+1)%8==0)cout <<endl;
    }
    cout << endl;

    emoji = "←";
    //8703-8592=111
    for(int i=0;i<112;i++){
        cout << emoji<<",";
        emoji=CalculateEmoji(emoji);
        if((i+1)%8==0)cout <<endl;
    }
    cout << endl;

    emoji = "⌀";
    //9215-8960=255
    for(int i=0;i<256;i++){
        cout << emoji<<",";
        emoji=CalculateEmoji(emoji);
        if((i+1)%8==0)cout <<endl;
    }
    cout << endl;
    
    emoji = "⤀";
    //10623-10496=127
    for(int i=0;i<128;i++){
        cout << emoji<<",";
        emoji=CalculateEmoji(emoji);
        if((i+1)%8==0)cout <<endl;
    }
    cout << endl;
    emoji = "⬀";
    //11263-11008=255
    for(int i=0;i<256;i++){
        cout << emoji<<",";
        emoji=CalculateEmoji(emoji);
        if((i+1)%8==0)cout <<endl;
    }
    cout << endl;

    emoji = "🀀";
    //127231-126976=255
    for(int i=0;i<256;i++){
        cout << emoji<<",";
        emoji=CalculateEmoji(emoji);
        if((i+1)%8==0)cout <<endl;
    }
    cout << endl;
    return;
}


string EmojiRemover(string sentence)
{
    string emoji = "🌀";
    //  129535-127744=1791
    for(int i=0;i<1792;i++){
        emoji=CalculateEmoji(emoji);
        sentence = regex_replace(sentence,regex(emoji),"");
    }
    return sentence;
}


int main() {
    string str="hello🧐,world.🧠";

    cout << EmojiRemover(str) << endl;
    return 0;
}