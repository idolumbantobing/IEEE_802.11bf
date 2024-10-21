/*
 * Copyright (c) 2018 University of Washington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef HE_CONFIGURATION_H
#define HE_CONFIGURATION_H

#include "ns3/nstime.h"
#include "ns3/object.h"

namespace ns3
{

/**
 * \brief HE configuration
 * \ingroup wifi
 *
 * This object stores HE configuration information, for use in modifying
 * AP or STA behavior and for constructing HE-related information elements.
 *
 */
class HeConfiguration : public Object
{
  public:
    HeConfiguration();

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \param guardInterval the supported HE guard interval
     */
    void SetGuardInterval(Time guardInterval);
    /**
     * \return the supported HE guard interval
     */
    Time GetGuardInterval() const;
    /**
     * \param bssColor the BSS color
     */
    void SetBssColor(uint8_t bssColor);
    /**
     * \return the BSS color
     */
    uint8_t GetBssColor() const;
    /**
     * \param maxTbPpduDelay the maximum TB PPDU delay
     */
    void SetMaxTbPpduDelay(Time maxTbPpduDelay);
    /**
     * \return the maximum TB PPDU delay
     */
    Time GetMaxTbPpduDelay() const;
    /**
     * \param size the MPDU buffer size to receive A-MPDUs
     */
    void SetMpduBufferSize(uint16_t size);
    /**
     * \return the MPDU buffer size to receive A-MPDUs
     */
    uint16_t GetMpduBufferSize() const;

    /*
    *************************************
    Attempt to add Channel Sounding from ns3.37
    new public configurations for HE
    *************************************
    */

    /**
     * \param ng subcarrier grouping parameter Ng for SU channel sounding feedback
     */
    void SetNgforSuFeedback(uint8_t ng);
    /**
     * \return subcarrier grouping parameter Ng for SU channel sounding feedback
     */
    uint8_t GetNgforSuFeedback() const;

    /**
     * \param ng subcarrier grouping parameter Ng for MU channel sounding feedback
     */
    void SetNgforMuFeedback(uint8_t ng);
    /**
     * \return subcarrier grouping parameter Ng for MU channel sounding feedback
     */
    uint8_t GetNgforMuFeedback() const;

    /**
     * \param codebookSize codebook size for SU beamforming report
     */
    void SetCodebookSizeforSu(std::string codebookSize);
    /**
     * \return codebook size for SU beamforming report
     */
    std::string GetCodebookSizeforSu() const;

    /**
     * \param codebookSize codebook size for MU beamforming report
     */
    void SetCodebookSizeforMu(std::string codebookSize);

    /**
     * \return codebook size for MU beamforming report
     */
    std::string GetCodebookSizeforMu() const;

    /**
     * Set to the maximum supported Nc for an HE compressed beamforming/CQI report minus 1.
     *
     * \param nc max Nc for beamforming report
     */
    void SetMaxNc(uint8_t nc);

    /**
     * Return max Nc for beamforming report
     *
     * \return max Nc for beamforming report
     */
    uint8_t GetMaxNc() const;

  private:
    Time m_guardInterval;      //!< Supported HE guard interval
    uint8_t m_bssColor;        //!< BSS color
    Time m_maxTbPpduDelay;     //!< Max TB PPDU delay
    uint16_t m_mpduBufferSize; //!< MPDU buffer size
    uint8_t m_muBeAifsn;       //!< AIFSN for BE in MU EDCA Parameter Set
    uint8_t m_muBkAifsn;       //!< AIFSN for BK in MU EDCA Parameter Set
    uint8_t m_muViAifsn;       //!< AIFSN for VI in MU EDCA Parameter Set
    uint8_t m_muVoAifsn;       //!< AIFSN for VO in MU EDCA Parameter Set
    uint16_t m_muBeCwMin;      //!< CWmin for BE in MU EDCA Parameter Set
    uint16_t m_muBkCwMin;      //!< CWmin for BK in MU EDCA Parameter Set
    uint16_t m_muViCwMin;      //!< CWmin for VI in MU EDCA Parameter Set
    uint16_t m_muVoCwMin;      //!< CWmin for VO in MU EDCA Parameter Set
    uint16_t m_muBeCwMax;      //!< CWmax for BE in MU EDCA Parameter Set
    uint16_t m_muBkCwMax;      //!< CWmax for BK in MU EDCA Parameter Set
    uint16_t m_muViCwMax;      //!< CWmax for VI in MU EDCA Parameter Set
    uint16_t m_muVoCwMax;      //!< CWmax for VO in MU EDCA Parameter Set
    Time m_beMuEdcaTimer;      //!< Timer for BE in MU EDCA Parameter Set
    Time m_bkMuEdcaTimer;      //!< Timer for BK in MU EDCA Parameter Set
    Time m_viMuEdcaTimer;      //!< Timer for VI in MU EDCA Parameter Set
    Time m_voMuEdcaTimer;      //!< Timer for VO in MU EDCA Parameter Set

    /*
    *************************************
    Attempt to add Channel Sounding from ns3.37
    new private configurations and attributes for HE
    *************************************
    */

    uint8_t m_ngforSuFeedback;       //!< Subcarrier grouping parameter Ng for SU
                                     //!< feedback
    uint8_t m_ngforMuFeedback;       //!< Enable subcarrier grouping parameter Ng to be 16 for MU
                                     //!< feedback
    std::string m_codebookSizeforSu; //!< Codebook size for SU feedback
    std::string m_codebookSizeforMu; //!< Codebook size for MU feedback
    uint8_t m_maxNc;                 //!< max Nc for beamforming report
};

} // namespace ns3

#endif /* HE_CONFIGURATION_H */
