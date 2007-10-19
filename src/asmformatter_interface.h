#ifndef __ASMFORMATTER_INTERFACE_H
#define __ASMFORMATTER_INTERFACE_H

class AsmFormatterInterface
{
public:
    AsmFormatterInterface() {}
    virtual ~AsmFormatterInterface() {}


private:
    AsmFormatterInterface( const AsmFormatterInterface& source);
    void operator = ( const AsmFormatterInterface& source);
};


#endif // __ASMFORMATTER_INTERFACE_H
