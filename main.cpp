#include <Regex.hpp>
#include <iostream>
#include <ctime>

void    print_match(ft::Regex r, const char *str)
{
    
    if (r.match(str))
    {
        for (size_t i = 0; i < r.groups.size(); i++)
            std::cout << r.groups[i] << " | ";
        std::cout << std::endl;
    }
    else
        std::cout << "No match" << std::endl;
}    

void    print_match(const char *regex, const char *str)
{
    ft::Regex r(regex);
    
    if (r.match(str))
    {
        for (size_t i = 0; i < r.groups.size(); i++)
            std::cout << r.groups[i] << " | ";
        std::cout << std::endl;
    }
    else
        std::cout << "No match" << std::endl;
}

void    benchmark(const char *regex, const char *str, int times = 100)
{
    double matching = 0, compiling = 0;
    clock_t start, end;
    
    for (int i = 0; i < times; i++)
    {
        start = clock();
        ft::Regex r(regex);
        end = clock();
        compiling += (double)(end - start) * 1000 / CLOCKS_PER_SEC;
        start = clock();
        r.match(str);
        end = clock();
        matching += (double)(end - start)* 100 / CLOCKS_PER_SEC;
    }
    std::cout << "Compiling: " << compiling / times << "ms | ";
    std::cout << "Matching: " << matching / times << "ms | ";
    print_match(regex, str);
    // std::cout << std::endl;
}



int main()
{
    benchmark("(\\w+)\\s(\\w+)\\s(\\w+)", "Hello\tWorld Again");
    
    benchmark("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b", "this is my not and ip: 192.168.1.999 but this an  ip: 192.168.1.1");
    
    benchmark("(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))", "2001:db8:3333:4444:5555:6666:7777:8888");
    
    benchmark("(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))", "this is not an ip v6 m001:dbZ8:3333:4444:5555:6666:7777:8888 but this is 2001:db8:3333:4444:CCCC:DDDD:EEEE:FFFF");

    benchmark("(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))", "this is not an ip v6 m001:dZb8:3333:4444:5555:6666:7777:8888 but this is");

    benchmark("https?:\\/\\/(?:[-\\w]+\\.)?([-\\w]+)\\.\\w+(?:\\.\\w+)?\\/?.*", "https://www.google.com/search?q=this+is+not+an+ip+v6+m001:dbZ8:3333:4444:5555:6666:7777:8888+but+this+is+2001:db8:3333:4444:CCCC:DDDD:EEEE:FFFF");

    benchmark("\\?php[ \\t]eval\\(base64_decode\\(\\'(([A-Za-z0-9+/]{4})*([A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)?){1}\\'\\)\\)\\;", "<?php eval(base64_decode('YW55IGNhcm5hbCBwbGVhc3VyZS4='));");
    

    benchmark("\\+?\\d{1,3}?[- .]?\\(?(?:\\d{2,3})\\)?[- .]?\\d\\d\\d[- .]?\\d\\d\\d\\d", "123-456-7890");
    benchmark("\\+?\\d{1,3}?[- .]?\\(?(?:\\d{2,3})\\)?[- .]?\\d\\d\\d[- .]?\\d\\d\\d\\d", "1234567890");
    benchmark("\\+?\\d{1,3}?[- .]?\\(?(?:\\d{2,3})\\)?[- .]?\\d\\d\\d[- .]?\\d\\d\\d\\d", "123.456.7890");
    benchmark("\\+?\\d{1,3}?[- .]?\\(?(?:\\d{2,3})\\)?[- .]?\\d\\d\\d[- .]?\\d\\d\\d\\d", "123 456 7890");
    benchmark("\\+?\\d{1,3}?[- .]?\\(?(?:\\d{2,3})\\)?[- .]?\\d\\d\\d[- .]?\\d\\d\\d\\d", "1243 456 7890");
    benchmark("\\+?\\d{1,3}?[- .]?\\(?(?:\\d{2,3})\\)?[- .]?\\d\\d\\d[- .]?\\d\\d\\d\\d", "+1 243 456 7890");

    benchmark("^[ \\s]+|[ \\s]+$", "hello world");
    benchmark("^[ \\s]+|[ \\s]+$", "hello world ");
    benchmark("^[ \\s]+|[ \\s]+$", " hello world");

    benchmark("\\< *[img][^\\>]*[src] *= *[\"\']{0,1}([^\"\'\\ >]*)", "<img src=\"http://www.google.com/images/srpr/logo3w.png\" />");
    benchmark("\\< *[img][^\\>]*[src] *= *[\"\']{0,1}([^\"\'\\ >]*)", "<!DOCTYPE html><html lang=\"en\"><head>    <meta charset=\"UTF-8\">    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">    <title>Document</title></head><body>    <div>        <img src=\"img/1.jpg\" alt=\"\">        <ul>            <li>1</li>            <li>2</li>            <li>3</li>            <li>4</li>            <li>5</li>        </ul></body></html>");

    benchmark("(?:(?:31(\\/|-|\\.)(?:0?[13578]|1[02]))\\1|(?:(?:29|30)(\\/|-|\\.)(?:0?[1,3-9]|1[0-2])\\2))(?:(?:1[6-9]|[2-9]\\d)?\\d{2})$|^(?:29(\\/|-|\\.)0?2\\3(?:(?:(?:1[6-9]|[2-9]\\d)?(?:0[48]|[2468][048]|[13579][26])|(?:(?:16|[2468][048]|[3579][26])00))))$|^(?:0?[1-9]|1\\d|2[0-8])(\\/|-|\\.)(?:(?:0?[1-9])|(?:1[0-2]))\\4(?:(?:1[6-9]|[2-9]\\d)?\\d{2})", "31/12/2014");
    benchmark("(?:(?:31(\\/|-|\\.)(?:0?[13578]|1[02]))\\1|(?:(?:29|30)(\\/|-|\\.)(?:0?[1-9]|1[0-2])\\2))(?:(?:1[6-9]|[2-9]\\d)?\\d{2})$|^(?:29(\\/|-|\\.)0?2\\3(?:(?:(?:1[6-9]|[2-9]\\d)?(?:0[48]|[2468][048]|[13579][26])|(?:(?:16|[2468][048]|[3579][26])00))))$|^(?:0?[1-9]|1\\d|2[0-8])(\\/|-|\\.)(?:(?:0?[1-9])|(?:1[0-2]))\\4(?:(?:1[6-9]|[2-9]\\d)?\\d{2})", "31/10/2019");

    benchmark("(?:(?:31(\\/|-|\\.)(?:0?[13578]|1[02]))\\1|(?:(?:29|30)(\\/|-|\\.)(?:0?[1-9]|1[0-2])\\2))(?:(?:1[6-9]|[2-9]\\d)?\\d{2})$|^(?:29(\\/|-|\\.)0?2\\3(?:(?:(?:1[6-9]|[2-9]\\d)?(?:0[48]|[2468][048]|[13579][26])|(?:(?:16|[2468][048]|[3579][26])00))))$|^(?:0?[1-9]|1\\d|2[0-8])(\\/|-|\\.)(?:(?:0?[1-9])|(?:1[0-2]))\\4(?:(?:1[6-9]|[2-9]\\d)?\\d{2})", "32/10/2019");


    benchmark("https?:\\/\\/(?:youtu\\.be\\/|(?:[a-z]{2,3}\\.)?youtube\\.com\\/watch(?:\\?|#\\!)v=)([\\w-]{11}).*", "http://youtu.be/dQw4w9WgXcQ");
    benchmark("https?:\\/\\/(?:youtu\\.be\\/|(?:[a-z]{2,3}\\.)?youtube\\.com\\/watch(?:\\?|#\\!)v=)([\\w-]{11}).*", "https://www.youtube.com/watch?v=4m7ubrdbWQU&ab_channel=DmitrySoshnikov");

    
    benchmark("\\b(?:4[0-9]{12}(?:[0-9]{3})?|5[1-5][0-9]{14}|6(?:011|5[0-9][0-9])[0-9]{12}|3[47][0-9]{13}|3(?:0[0-5]|[68][0-9])[0-9]{11}|(?:2131|1800|35\\d{3})\\d{11})\\b", "f you're looking for random paragraphs, you've come to the right place. When a random word or a random sentence isn't quite enough, the 4650398256543094 next logical step is to find a random paragraph. We created the Random Paragraph Generator with you in mind. The process is quite simple. Choose the number of random paragraphs you'd like to see and click the button. Your chosen number of paragraphs will instantly appear.");
    benchmark("\\b(?:4[0-9]{12}(?:[0-9]{3})?|5[1-5][0-9]{14}|6(?:011|5[0-9][0-9])[0-9]{12}|3[47][0-9]{13}|3(?:0[0-5]|[68][0-9])[0-9]{11}|(?:2131|1800|35\\d{3})\\d{11})\\b", "01234567896352");

    return 0;
}