


#ifndef __PCF8574_H__
#define __PCF8574_H__ 



//
//
//
template <typename ChannelInBufferType, typename ChannelOutBufferType, uint8_t address>
class PCF8574
{
public:

    PCF8574(ChannelInBufferType& _inChannel, ChannelOutBufferType& _outChannel) :
        inChannel(_inChannel),
        outChannel(_outChannel)
    {
    }


    void SetOutputs( uint8_t bits )
    {
    }

private:

    ChannelInBufferType&    inChannel;
    ChannelOutBufferType&   outChannel;


};



#endif



