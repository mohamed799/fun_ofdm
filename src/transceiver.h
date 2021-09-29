/*!  \file transceiver.h
 *   \brief Header file for the transceiver class.
 * 
 *   This class combines functionality from the transmitter and receiver classes to form
 *   a device which mimics a relay in a transcoding channel. In initial use, it is
 *   designed to be used such that it receives a transmitted payload, stores it and then
 *   transmits it on user command.
 */

#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <vector>
#include <semaphore.h>
#include "usrp.h"
#include "rates.h"
#include "frame_builder.h"
#include "receiver_chain.h"

#define NUM_RX_SAMPLES 4096

namespace fun {

  class transceiver {
    public:
      /*!
       * \brief Constructor for the receiver with raw parameters
       * \param callback Function pointer to the callback function where received packets are passed
       * \param freq [Optional] Center frequency
       * \param samp_rate [Optional] Sample Rate
       * \param rx_gain [Optional] Receive Gain
       * \param device_addr [Optional] IP address of USRP device
       *
       *  Defaults to:
       *  - center freq -> 5.72e9 (5.72 GHz)
       *  - sample rate -> 5e6 (5 MHz)
       *  - rx gain -> 20
       *  - device ip address -> "" (empty string will default to letting the UHD api
       *    automatically find an available USRP)
       *  - *Note:
       *    + tx_gain -> 20 even though it is irrelevant for the receiver
       *    + amp -> 1.0 even though it is irrelevant for the receiver
       */
      transceiver(void(*callback)(std::vector<std::vector<unsigned char> > packets), double freq = 5.72e9, double samp_rate = 5e6, double rx_gain = 20, std::string device_addr = "");

      /*!
       * \brief Constructor for the receiver that uses the usrp_params struct
       * \param callback Function pointer to the callback function where received packets are passed
       * \param params [Optional] The usrp parameters you want to use for this receiver.
       *
       *  Defaults to:
       *  - center freq -> 5.72e9 (5.72 GHz)
       *  - sample rate -> 5e6 (5 MHz)
       *  - tx gain -> 20
       *  - rx gain -> 20 (although this is irrelevant for the transmitter)
       *  - device ip address -> "" (empty string will default to letting the UHD api
       *    automatically find an available USRP)
       */
      transceiver(void(*callback)(std::vector<std::vector<unsigned char> > packets), usrp_params params = usrp_params());

      /*!
       * \brief Pauses the receiver thread.
       */
      void pause();

      /*!
       * \brief Resumes the receiver thread after it has been paused.
       */
      void resume();

      /*!
       * \brief Send a single PHY frame at the given PHY Rate
       * \param payload The data to be transmitted (i.e. the MPDU)
       * \param phy_rate [Optional] The PHY data rate to transmit at - defaults to 1/2 BPSK
       *
       *  This function uses the usrp::send_burst_sync() function which means that this function
       *  blocks until the packet is done transmitting.
       */

      void send_frame(std::vector<unsigned char> payload, Rate phy_rate = RATE_1_2_BPSK);

      /*!
       *  This function loops forever (unless it is paused) pulling samples from the USRP and passing them through the
       *  receiver chain. It then passes any successfully decoded packets to the callback function for the user
       *  to process further. This function can be paused by the user by calling the receiver::pause() function,
       *  presumably so that the user can transmit packets over the air using the transmitter. Once the user is finished
       *  transmitting he/she can resume the receiver by called the receiver::resume() function. These two functions use
       *  an internal semaphore to block the receiver code execution while in the paused state.
       */

      std::vector<std::vector<unsigned char>> received_packets;

    private:

      usrp m_usrp; //!< The usrp object used to send the generated frames over the air

      frame_builder m_frame_builder; //!< The frame builder object used to generate the frames
      void receiver_chain_loop(); //!< Infinite while loop where samples are received from USRP and processed by the receiver_chain

      void (*m_callback)(std::vector<std::vector<unsigned char> > packets); //!< Callback function pointer

      receiver_chain m_rec_chain; //!< The receiver chain object used to detect & decode incoming frames

      std::vector<std::complex<double> > m_samples; //!< Vector to hold the raw samples received from the USRP and passed into the receiver_chain

      std::thread m_rec_thread; //!< The thread that the receiver chain runs in

      sem_t m_pause; //!< Semaphore used to pause the receiver thread

  };

}

#endif // TRANSCEIVER_H
