#include <iostream>

#include <transmitter.h>
#include <receiver.h>
#include <transceiver.h>

void callback(std::vector<std::vector<unsigned char> > payloads);
bool set_realtime_priority();

using namespace std;
using namespace fun;

int main()
{
    set_realtime_priority();

    usrp_params params = usrp_params();

    params.device_addr = "addr=172.23.202.2";
    //transmitter tx = transmitter(params);

    params.device_addr = "addr=172.23.202.2";
    transceiver tx_rx = transceiver(&callback, params);
    //tx_rx.pause();

    params.device_addr = "addr=172.23.201.2";
    receiver rx = receiver(&callback, params);

    std::string s = "Hello World";
    cout << s << std::endl;

    std::vector<unsigned char> data = std::vector<unsigned char>(12);
    memcpy(&data[0], &s[0], 12);

    //rx.pause();
    //tx_rx.pause();
    //sleep(1);
    tx_rx.send_frame(data);
    //rx.pause();

    while(1)
    {
	//tx_rx.pause();
        //rx.pause();
        sleep(1);
	rx.pause();
        cout << "Sending \"Hello World\" " << std::endl;
        tx_rx.send_frame(data);
        rx.resume();
        //tx_rx.pause();

        //for(int i = 0; i < tx_rx.received_packets.size(); i++) {
          //rx.resume();
          //tx_rx.send_frame(tx_rx.received_packets[i]);
          //sleep(4);
        //} 
    }

    return 0;
}

void callback(std::vector<std::vector<unsigned char> > payloads)
{
    for(int i = 0; i < payloads.size(); i++) {
      std::cout << "Received a packet!" << std::endl;
      std::string s(payloads[i].begin(), payloads[i].end());
      std::cout << s << std::endl;
    }
}

/*!
 * \brief Attempt to set real time priority for thread scheduling
 * \return Whether or not real time priority was succesfully set.
 */
bool set_realtime_priority()
{
    // Get the current thread
    pthread_t this_thread = pthread_self();

    // Set priority to SCHED_FIFO
    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_RR);
    if (pthread_setschedparam(this_thread, SCHED_RR, &params) != 0)
    {
        std::cout << "Unable to set realtime priority. Did you forget to sudo?" << std::endl;
        return false;
    }

    return true;
}
