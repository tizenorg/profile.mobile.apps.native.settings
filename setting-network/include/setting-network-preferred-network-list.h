/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
 *
 * Contact: MyoungJune Park <mj2004.park@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef _SETTINGS_NETWORK_PREFERRED_NETWORK_LIST_H_
#define _SETTINGS_NETWORK_PREFERRED_NETWORK_LIST_H_

#include <setting-network.h>
#include <setting-network-preferred-network.h>

typedef enum {

    SETTING_PREFERRED_NETWORK_TYPE_UNKNOWN,					/**< Network type unknown */

    SETTING_PREFERRED_NETWORK_TYPE_GSM_900,					/**< Network type Gsm 900 */

    SETTING_PREFERRED_NETWORK_TYPE_DCS_1800,				/**< Network type DCS 1800 */

    SETTING_PREFERRED_NETWORK_TYPE_PCS_1900,				/**< Network type Pcs 1900 */

    SETTING_PREFERRED_NETWORK_TYPE_GSM_SAT,					/**< Network type Gsm SAT */

    SETTING_PREFERRED_NETWORK_TYPE_GSM_850,					/**< Network type Gsm 850*/

    SETTING_PREFERRED_NETWORK_TYPE_UMTS,					/**< Network type UMTS */
    SETTING_PREFERRED_NETWORK_TYPE_MAX
} setting_network_preferred_network_list;

typedef struct {
	int mcc;
	int mnc;
	setting_network_preferred_network_list nw_type;
	char *short_name_ptr;
	char *country_code;

} settings_preferred_network_table;

/** @todo does static preffered network table need to update? */
static const settings_preferred_network_table network_table[] = {

	/****************
	 **** Greece ****
	 ****************/

	/* COSMOTE */
	{
		202, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GR COSMOTE",
		"COSMOTE"
	},

	/* COSMOTE */
	{
		202, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "GR COSMOTE",
		"COSMOTE"
	},

	/* Vodafone */
	{
		202, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone GR",
		"Vodafone"
	},

	/* Info Quest - Commercial & Industrial SA */
	{
		202, 9, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "GR Q-TELECOM",
		"Info Quest - Commercial & Industrial SA"
	},

	/* STET HELLAS */
	{
		202, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GR TELESTET",
		"STET HELLAS"
	},

	/*********************
	 **** Netherlands ****
	 *********************/

	/* Vodafone Libertel N.V */
	{
		204, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone NL",
		"Vodafone Libertel N.V"
	},

	/* Vodafone Libertel N.V */
	{
		204, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "vodafone NL",
		"Vodafone Libertel N.V"
	},

	/* KPN Mobile The Netherlands BV */
	{
		204, 8, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "NL KPN",
		"KPN Mobile The Netherlands BV"
	},

	/* KPN Mobile The Netherlands BV */
	{
		204, 8, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "NL KPN",
		"KPN Mobile The Netherlands BV"
	},

	/* Telfort Mobiel B.V. */
	{
		204, 12, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "NL Telfort",
		"Telfort B.V."
	},

	/* T-Mobile Netherlands */
	{
		204, 16, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T-Mobile NL",
		"T-Mobile Netherlands"
	},

	/* T-Mobile Netherlands */
	{
		204, 16, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "T-Mobile NL",
		"T-Mobile NL"
	},

	/* Orange Nederland N.V. */
	{
		204, 20, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Orange NL",
		"Orange Nederland N.V."
	},

	/*****************
	 **** Belgium ****
	 *****************/

	/* Belgacom Mobile */
	{
		206, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BEL PROXIMUS",
		"Belgacom Mobile"
	},

	/* Belgacom Mobile */
	{
		206, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "BEL PROXIMUS",
		"Belgacom Mobile"
	},

	/* Mobistar S.A. */
	{
		206, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "B mobistar",
		"Mobistar S.A."
	},

	/* Mobistar S.A. */
	{
		206, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "B mobistar",
		"Mobistar"
	},

	/* Mobistar S.A. */
	{
		206, 10, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "B mobistar",
		"Mobistar"
	},

	/* KPN Orange Belgium NV/SA */
	{
		206, 20, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "BASE",
		"KPN Orange Belgium NV/SA"
	},

	/****************
	 **** France ****
	 ****************/

	/* Orange France */
	{
		208, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Orange F",
		"Orange France"
	},

	/* Orange France */
	{
		208, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Orange F",
		"Orange France"
	},

	/* SFR */
	{
		208, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "F SFR",
		"SFR - CEGETEL"
	},

	/* SFR */
	{208, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "F SFR", "SFR"},

	/* Bouygues Telecom */
	{
		208, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BOUYGTEL",
		"Bouygues Telecom"
	},

	/* Bouygues Telecom */
	{
		208, 20, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "BOUYGTEL",
		"Bouygues Telecom"
	},

	/*****************
	 **** Andorra ****
	 *****************/

	/* Mobiland */
	{
		213, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "STA-MOBILAND",
		"Servei De Tele. DAndorra"
	},

	/***************
	 **** Spain ****
	 ***************/

	/* Airtel Movil S.A */
	{
		214, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone ES",
		"ES vodafone"
	},

	/* Airtel Movil S.A */
	{
		214, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "vodafone ES",
		"ES vodafone"
	},

	/* Telefonica Moviles */
	{
		214, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MOVISTAR",
		"Telefonica Moviles"
	},

	/* Telefonica Moviles */
	{
		214, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MOVISTAR",
		"Telefonica Moviles"
	},

	/* Retevision Movil S.A */
	{
		214, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "E AMENA",
		"Retevision Movil S.A"
	},

	/* Retevision Movil S.A */
	{
		214, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "E AMENA",
		"Retevision Movil S.A"
	},

	/* Xfera Moviles SA */
	{
		214, 4, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "Xfera",
		"Xfera Moviles SA"
	},

	/* Telefonica Moviles */
	{
		214, 7, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MOVISTAR",
		"Telefonica Moviles"
	},

	/* Telefonica Moviles */
	{
		214, 7, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MOVISTAR",
		"Telefonica Moviles"
	},

	/* Telefonica Moviles */
	{214, 77, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MOVISTAR", "Telefonica Moviles"},	/* added for 214 77 display problem for Movistar */

	/*****************
	 **** Hungary ****
	 *****************/

	/* Pannon GSM Telecoms Plc */
	{
		216, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "H PANNON GSM",
		"Pannon GSM Telecoms Plc"
	},

	/* Pannon GSM Telecoms Plc */
	{
		216, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "H PANNON GSM",
		"Pannon GSM Telecoms Plc"
	},

	/* T-Mobile Hungary Telecommunications Co. Ltd */
	{
		216, 30, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "T-Mobile H",
		"T-Mobile Hungary Telecommunications Co. Ltd"
	},

	/* T-Mobile Hungary Telecommunications Co. Ltd */
	{
		216, 30, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T-Mobile H",
		"T-Mobile Hungary Telecommunications Co. Ltd"
	},

	/* Vodafone Hungary Ltd */
	{
		216, 70, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone HU",
		"Vodafone Hungary Ltd"
	},

	/* Vodafone Hungary Ltd */
	{
		216, 70, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "vodafone HU",
		"Vodafone Hungary Ltd"
	},

	/****************
	 **** Bosnia ****
	 ****************/

	/* Eronet Mobile Communications Ltd */
	{
		218, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BA-ERONET",
		"Eronet Mobile Communications Ltd"
	},

	/* RS Telecommunications  JSC Banja Luka */
	{
		218, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MOBI'S",
		"RS Telecommunications  JSC Banja Luka"
	},

	/* Public Enterprise BH Telecom Sarajevo */
	{
		218, 90, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BH GSMBIH",
		"Public Enterprise BH Telecom Sarajevo"
	},

	/*****************
	 **** Croatia ****
	 *****************/

	/* HT Mobile Communications, LLC */
	{
		219, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "T-Mobile HR",
		"HT Mobile Communications, LLC"
	},

	/* Tele2 d.o.o za telekomunikacijske uluge */
	{219, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TELE2", "Tele2"},

	/* Vip-NET GSM d.o.o. */
	{
		219, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "HR VIP",
		"VIPnet d.o.o."
	},

	/********************
	 **** Yugoslavia ****
	 ********************/

	/* Mobile Telecommunications */
	{
		220, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "YU MOBTEL",
		"Mobile Telecommunications"
	},

	/* Mobile Telecommunications */
	{
		220, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "YU MOBTEL",
		"Mobile Telecommunications"
	},

	/* ProMonte GSM */
	{
		220, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ProMonte",
		"ProMonte GSM"
	},

	/* ProMonte GSM */
	{
		220, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "ProMonte",
		"ProMonte GSM"
	},

	/* Telekom Srbija a.d. */
	{
		220, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "YUG 03",
		"Telekom Srbija"
	},

	/* Monet D.O.O */
	{
		220, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MONET",
		"Monet Ltd, Podgorica"
	},

	/***************
	 **** Italy ****
	 ***************/

	/* Telecom Italia Mobile */
	{
		222, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, " I TIM",
		"Telecom Italia Mobile"
	},

	/* Telecom Italia Mobile */
	{
		222, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "I TIM",
		"Telecom Italia Mobile"
	},

	/* Omnitel Pronto Italia is Vodafone IT now */
	{
		222, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone IT",
		"Omnitel Pronto Italia"
	},

	/* Omnitel Pronto Italia is Vodafone IT now*/
	{
		222, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "vodafone IT",
		"Omnitel Pronto Italia"
	},

	/* Wind Telecomunicazioni SpA */
	{
		222, 88, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "I WIND",
		"Wind Telecomunicazioni SpA"
	},

	/* Wind Telecomunicazioni SpA*/
	{
		222, 88, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "I WIND",
		"Wind Telecomunicazioni SpA"
	},

	/* Wind Telecomunicazioni SpA*/
	{
		222, 88, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "I WIND",
		"Wind Telecomunicazioni SpA"
	},

	/* Blu SpA */
	{222, 98, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IT Blu", "Blu SpA"},

	/* Hutchison 3 */
	{222, 99, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "3 ITA", "H3G"},

	/*****************
	 **** Romania ****
	 *****************/

	/* MobiFon S.A */
	{
		226, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RO CONNEX",
		"MobiFon S.A"
	},

	/* Cosmorom */
	{
		226, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RO Cosmorom",
		"Cosmorom"
	},

	/* Cosmorom */
	{
		226, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "RO Cosmorom",
		"Cosmorom"
	},

	/* Orange Romania SA */
	{
		226, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RO ORANGE",
		"Orange Romania SA"
	},

	/* Orange Romania SA */
	{
		226, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "RO ORANGE",
		"Orange Romania SA"
	},

	/*********************
	 **** Switzerland ****
	 *********************/

	/* Swisscom Mobile Ltd */
	{
		228, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SWISSCOM",
		"Swisscom Mobile Ltd"
	},

	/* Swisscom Mobile Ltd */
	{
		228, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SWISSCOM",
		"Swisscom Mobile Ltd"
	},

	/* TDC Switzerland AG */
	{
		228, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "sunrise",
		"TDC Switzerland AG"
	},

	/* TDC Switzerland AG */
	{
		228, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "sunrise",
		"TDC Switzerland AG"
	},

	/* Orange Communications S.A */
	{
		228, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "orange CH",
		"Orange Communications S.A"
	},

	/* In &Phone SA */
	{
		228, 7, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "In&Phone",
		"in&phone"
	},

	/* Tele2 Telecommunications Services AG */
	{
		228, 8, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T2",
		"Tele2 Switzerland"
	},

	/************************
	 **** Czech Republic ****
	 ************************/

	/* T- Mobile Networks */
	{
		230, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "T-Mobile CZ",
		"T-Mobile Czech Republic a.s."
	},

	/* T- Mobile Networks */
	{
		230, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T-Mobile CZ",
		"T-Mobile Czech Republic a.s."
	},

	/* EuroTel Praha Spol. sro */
	{
		230, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "EUROTEL - CZ",
		"Eurotel Praha, spol. s.r.o."
	},

	/* EuroTel Praha Spol. sro */
	{
		230, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "EUROTEL - CZ",
		"Eurotel Praha, spol. s.r.o."
	},

	/* Cesky Mobil a.s */
	{
		230, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "OSKAR",
		"Cesky Mobil a.s"
	},

	/* Cesky Mobil a.s */
	{
		230, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "OSKAR",
		"Cesky Mobil a.s"
	},

	/*************************
	 **** Slovak Republic ****
	 *************************/

	/* Orange Slovensko a.s. */
	{
		231, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Orange SK",
		"Orange Slovensko a.s."
	},

	/* EuroTel Bratislava a.s */
	{
		231, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "T-Mobile SK",
		"EuroTel Bratislava a.s"
	},

	/* EuroTel Bratislava a.s */
	{
		231, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T-Mobile SK",
		"EuroTel Bratislava a.s"
	},

	/* "T-Mobile Slovensko, a.s." */
	{
		231, 2, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "T-Mobile SK",
		"T-Mobile SK"
	},

	/*****************
	 **** Austria ****
	 *****************/

	/* MobilKom Austria AG */
	{
		232, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "A1",
		"MobilKom Austria AG"
	},

	/* MobilKom Austria AG */
	{
		232, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "A1",
		"MobilKom Austria AG"
	},

	/* MobilKom Austria AG */
	{
		232, 1, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "A1",
		"MobilKom Austria AG"
	},

	/* T-Mobile Austria GmbH */
	{
		232, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "T-Mobile A",
		"T-Mobile Austria GmbH"
	},

	/* T-Mobile Austria GmbH */
	{
		232, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T-Mobile A",
		"T-Mobile Austria GmbH"
	},

	/* Connect Austria */
	{
		232, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "one",
		"Connect Austria"
	},

	/* ONE GMBH */
	{232, 5, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "one", "ONE GMBH"},

	/* Tele.ring Telekom Service GmbH & Co KG */
	{
		232, 7, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "A tele.ring",
		"Tele.ring Telekom Service GmbH & Co KG"
	},

	/* Hutchison 3G Austria GmbH */
	{
		232, 10, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "3 AT",
		"Hutchison 3G Austria GmbH"
	},

	/************************
	 **** United Kingdom ****
	 ************************/

	/* BTCellnet */
	{
		234, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "O2 - UK",
		"BTCellnet"
	},

	/* BTCellnet */
	{
		234, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "O2 - UK",
		"BTCellnet"
	},

	/* O2 (UK) Limited */
	{
		234, 10, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "O2 - UK ",
		"O2 (UK) Limited"
	},

	/* Vodafone Ltd */
	{234, 15, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone UK", "Vodafone Ltd"},	/* changed to vodafone UK */

	/* Vodafone Ltd */
	{
		234, 15, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "vodafone UK",
		"Vodafone Ltd"
	},

	/*  Hutchinson UK */
	{
		234, 20, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "3 UK",
		"Hutchison 3G UK Ltd"
	},

	/*  T-Mobile (UK) Limited */
	{
		234, 30, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T-Mobile UK",
		"T-Mobile (UK) Limited"
	},

	/*  T-Mobile (UK) Limited */
	{
		234, 31, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T-Mobile UK",
		"T-Mobile (UK) Limited"
	},

	/*  T-Mobile (UK) Limited */
	{
		234, 32, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T-Mobile UK",
		"T-Mobile (UK) Limited"
	},

	/* Orange PCS Ltd */
	{
		234, 33, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Orange",
		"Orange PCS Ltd"
	},

	/* Jersey Telecoms */
	{
		234, 50, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "JT GSM",
		"Jersey Telecoms"
	},

	/* Wave Telecom */
	{234, 50, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "JT GSM", "JT-Wave"},

	/* Wave Telecom */
	{234, 50, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "JT-Wave", "JT-Wave"},

	/* Cable & Wireless Guernsey Ltd */
	{
		234, 55, SETTING_PREFERRED_NETWORK_TYPE_GSM_900,
		"Cable & Wireless Guernsey", "Cable & Wireless Guernsey Ltd"
	},

	/* Manx Telecom */
	{
		234, 58, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Manx Pronto",
		"Manx Telecom"
	},

	/*****************
	 **** Denmark ****
	 *****************/

	/* TDC Mobil A/S */
	{
		238, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TDC MOBIL",
		"TDC Mobil A/S"
	},

	/* TDC Mobil A/S */
	{
		238, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TDC MOBIL",
		"TDC Mobil A/S"
	},

	/* Sonofon */
	{
		238, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "DK SONOFON",
		"Sonofon"
	},

	/* Sonofon */
	{
		238, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "DK SONOFON",
		"Sonofon"
	},

	/* HiTAPI_NETW_TYPE_UMTS Denmark ApS */
	{238, 6, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "3 DK ", "3 DK"},

	/* Telia A/S Denmark */
	{
		238, 20, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TELIA DK",
		"Telia A/S Denmark"
	},

	/* Orange A/S */
	{
		238, 30, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Orange",
		"Orange A/S"
	},

	/* Orange A/S */
	{
		238, 30, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Orange",
		"Orange A/S"
	},

	/****************
	 **** Sweden ****
	 ****************/

	/* Telia AB */
	{240, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "S Telia", "Telia AB"},

	/* Telia AB */
	{
		240, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "S Telia",
		"Telia AB"
	},

	/* Hi3G Access AB */
	{240, 2, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "3 SE", "HI3G Access AB"},

	/* Orange Sverige AB */
	{
		240, 3, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "Orange",
		"Orange Sverige AB"
	},

	/* HI3G Access AB */
	{
		240, 4, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "vodafone SE",
		"HI3G Access AB"
	},

	/* COMVIQ GSM shared with TeliaSonera MobileNetworks */
	{
		240, 5, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "Sweden 3G",
		"COMVIQ GSM"
	},

	/* Tele 2 AB */
	{
		240, 7, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "S COMVIQ",
		"Tele 2 AB"
	},

	/* Tele 2 AB */
	{
		240, 7, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "S COMVIQ",
		"Tele 2 AB"
	},

	/* Europolitan AB */
	{
		240, 8, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone SE",
		"Europolitan AB"
	},

	/* Europolitan AB */
	{
		240, 8, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "vodafone SE",
		"Europolitan AB"
	},

	/* Vodafone Sverige AB */
	{
		240, 8, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "vodafone SE",
		"Vodafone "
	},

	/* Swefour AB */
	{
		240, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SpringMobil SE",
		"Swefour AB"
	},

	/* Swefour AB */
	{
		240, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SpringMobil SE",
		"Swefour AB"
	},

	/****************
	 **** Norway ****
	 ****************/

	/* Telenor Mobil AS */
	{
		242, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "N Telenor",
		"Telenor Mobil AS"
	},

	/* Telenor Mobil AS */
	{
		242, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "N Telenor",
		"Telenor Mobil AS"
	},

	/* NETCOM AS */
	{
		242, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "N NetCom GSM",
		"NETCOM AS"
	},

	/* NETCOM AS */
	{
		242, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "N NetCom GSM",
		"NETCOM AS"
	},

	/* Teletopia Mobile Communications AS */
	{242, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T3", "Teletopia"},

	/*****************
	 **** Finland ****
	 *****************/

	/* Telia Mobile AB */
	{
		244, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "FI TELIA",
		"Telia Mobile AB Branch in Finland"
	},

	/* Elisa Corporation */
	{
		244, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "elisa",
		"Elisa Corporation"
	},

	/* Elisa Corporation */
	{
		244, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "elisa",
		"Elisa Corporation"
	},

	/* Elisa Corporation */
	{
		244, 5, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "elisa",
		"Elisa Corporation"
	},

	/* Finnet Group */
	{
		244, 9, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Finnet",
		"Finnet Group"
	},

	/* Finnet Networks Ltd */
	{
		244, 12, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "FI 2G",
		"Finnet Networks Ltd"
	},

	/* Finnet Networks Ltd */
	{
		244, 12, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "FI 2G",
		"Finnet Networks Ltd"
	},

	/* Finnet Networks Ltd */
	{
		244, 12, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "FI 2G",
		"Finnet Networks Ltd"
	},

	/* Alands Mobiltelefon A.B */
	{
		244, 14, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "FI AMT",
		"Alands Mobiltelefon Ab"
	},

	/* Sonera Mobile Networks Limited */
	{
		244, 91, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "FI SONERA",
		"Sonera Mobile Networks Limited"
	},

	/* Sonera Mobile Networks Limited */
	{
		244, 91, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "FI SONERA",
		"Sonera Mobile Networks Limited"
	},

	/* Sonera Mobile Networks Limited */
	{
		244, 91, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "FI SONERA",
		"Sonera Mobile Networks Limited"
	},

	/*******************
	 **** Lithuania ****
	 *******************/

	/* Omnitel */
	{
		246, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "OMNITEL LT",
		"OMNITEL"
	},

	/* UAB Bite GSM */
	{
		246, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LT BITE GSM",
		"UAB Bite GSM"
	},

	/* UAB Bite GSM */
	{
		246, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "LT BITE GSM",
		"UAB Bite GSM"
	},

	/* UAB TELE2 */
	{246, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TELE2", "UAB TELE2"},

	/* UAB TELE2 */
	{246, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TELE2", "UAB TELE2"},

	/****************
	 **** Latvia ****
	 ****************/

	/* Latvian Mobile Tel. Co. */
	{
		247, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LMT GSM",
		"Latvian Mobile Tel. Co."
	},

	/* Latvian Mobile Tel. Co. */
	{
		247, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "LMT GSM",
		"Latvian Mobile Tel. Co."
	},

	/* TELE2 */
	{247, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LV TELE2", "TELE2"},

	/* TELE2 */
	{247, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "LV TELE2", "TELE2"},

	/* LV BITE */
	{247, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LV BITE", "LV BITE"},

	/* SIA Bite Latvija */
	{
		247, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "LV BITE",
		"Bite Latvija"
	},

	/* SIA Bite Latvija */
	{
		247, 5, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "LV BITE",
		"Bite Latvija"
	},

	/*****************
	 **** Estonia ****
	 *****************/

	/* AS EMT */
	{
		248, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "EE EMT GSM",
		"AS EMT"
	},

	/* AS EMT */
	{
		248, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "EE EMT GSM",
		"AS EMT"
	},

	/* AS EMT */
	{248, 1, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "EMT", "EMT"},

	/* Radiolinja Eesti AS */
	{
		248, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "EE RLE GSM",
		"Radiolinja Eesti AS"
	},

	/* Radiolinja Eesti AS */
	{
		248, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "EE RLE GSM",
		"Radiolinja Eesti AS"
	},

	/* Elisa Mobiilsideteenused AS */
	{
		248, 2, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "elisa",
		"Elisa Mobiilsideteenused AS"
	},

	/* Tele2 Eesti AS */
	{
		248, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TELE2",
		"Tele2 Eesti AS"
	},

	/* Tele2 Eesti AS */
	{
		248, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TELE2",
		"Tele2 Eesti AS"
	},

	/****************
	 **** Russia ****
	 ****************/

	/* Mobile Telesystems */
	{
		250, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MTS-RUS",
		"Mobile Telesystems"
	},

	/* Mobile TeleSystems (MTS) */
	{250, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MTS-RUS", "MTS"},

	/* North-West GSM */
	{
		250, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MegaFon RUS",
		"MegaFon, Open Joint Stock Company"
	},

	/* North-West GSM */
	{
		250, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MegaFon RUS",
		"MegaFon, Open Joint Stock Company"
	},

	/* OJSC Mobile Communication Systems-Povolzhie */
	{
		250, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MegaFon RUS",
		"OJSC Mobile Communication Systems-Povolzhie"
	},

	/* Nizhegorodskaya Cellular Communications */
	{
		250, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS NCC",
		"Nizhegorodskaya Cellular Communications"
	},

	/* SIBCHALLENGE LTD */
	{
		250, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SIBCHALLENGE RUS",
		"SIBCHALLENGE LTD"
	},

	/* Mobile Communications Systems */
	{
		250, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS SCS",
		"Mobile Communications Systems"
	},

	/* Sayantelecom LLC */
	{
		250, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS SCS",
		"Sayantelecom LLC"
	},

	/* Siberian Cellular Systems-900 */
	{
		250, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SCS RUS",
		"Siberian Cellular Systems-900"
	},

	/* Yeniseitelecom */
	{
		250, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SCS RUS",
		"Yeniseitelecom"
	},

	/* Tomsk Cellular Communication Ltd */
	{
		250, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SCS",
		"Tomsk Cellular Communication Ltd"
	},

	/* BM Telecom Limited */
	{
		250, 7, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS BMT",
		"BM Telecom Limited"
	},

	/* Zao Smarts */
	{
		250, 7, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS SMARTS",
		"ZAO SMARTS"
	},

	/* Zao Smarts */
	{
		250, 7, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "RUS SMARTS",
		"ZAO SMARTS"
	},

	/* DonTeleCom */
	{
		250, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS DTC",
		"DonTeleCom"
	},

	/* Joint-stock company - Orensot */
	{
		250, 11, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ORENSOT",
		"Joint-stock company - Orensot"
	},

	/* Baykalwestcom */
	{
		250, 12, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS FEast",
		"Baykalwestcom"
	},

	/* Far Eastern Cellular Systems - 900 */
	{
		250, 12, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS Far East",
		"Far Eastern Cellular Systems - 900"
	},

	/* JSC Sakhalin Telecom Mobile */
	{
		250, 12, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "RUS-Sakhalin",
		"JSC Sakhalin Telecom Mobile"
	},

	/* Sibintertelecom */
	{
		250, 12, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SIBI-RUS",
		"Sibintertelecom"
	},

	/* Kuban- GSM Closed JSC */
	{
		250, 13, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS Kuban-GSM",
		"Kuban- GSM Closed JSC"
	},

	/* ZAO SMARTS */
	{
		250, 15, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SMARTS",
		"SMARTS-Ufa"
	},

	/* New Telephone Company */
	{
		250, 16, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS 16",
		"New Telephone Company"
	},

	/* Ermak RMS */
	{
		250, 17, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS 17",
		"Ermak RMS"
	},

	/* Bashcell, CJSC */
	{
		250, 19, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "RUS_BASHCELL",
		"Bashcell, CJSC "
	},

	/* CJSC Volgograd Mobile */
	{
		250, 19, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "RUS INDIGO",
		"CJSC Volgograd Mobile"
	},

	/* Cellular Communications of Udmurtia */
	{
		250, 20, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TELE2",
		"Cellular Communications of Udmurtia"
	},

	/* JSC Extel */
	{
		250, 28, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "EXTEL RUS",
		"JSC Extel"
	},

	/* JSC Uralsvyazinform */
	{
		250, 39, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Uraltel",
		"JSC Uralsvyazinform"
	},

	/* South Ural Cellular Telephone */
	{
		250, 39, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS 39",
		"South Ural Cellular Telephone"
	},

	/* Uraltel Ltd */
	{
		250, 39, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Uraltel",
		"Uraltel Ltd"
	},

	/* StavTeleSot */
	{
		250, 44, SETTING_PREFERRED_NETWORK_TYPE_GSM_900,
		"RUS North Caucasian GSM", "StavTeleSot"
	},

	/* Primtelefon */
	{
		250, 92, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Primetelefone RUS",
		"Primtelefon"
	},

	/* Primtelefon */
	{
		250, 92, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Primetelefone RUS",
		"Primtelefon"
	},

	/* Telecom XXI JSC */
	{
		250, 93, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RUS 93",
		"Telecom XXI JSC"
	},

	/* Telecom XXI JSC */
	{
		250, 93, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "RUS 93",
		"Telecom XXI JSC"
	},

	/* KB Impuls */
	{
		250, 99, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BeeLine",
		"KB Impuls"
	},

	/* KB Impuls */
	{
		250, 99, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "BeeLine",
		"KB Impuls"
	},

	/*****************
	 **** Ukraine ****
	 *****************/

	/* Ukrainian Mobile Comms */
	{
		255, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "UA UMC",
		"Ukrainian Mobile Communications"
	},

	/* Ukrainian Mobile Comms */
	{
		255, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "UA UMC",
		"Ukrainian Mobile Communications"
	},

	/* Ukrainian Radio Systems */
	{
		255, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "WellCOM",
		"Ukrainian Radio Systems"
	},

	/* Ukrainian Radio Systems */
	{
		255, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "WellCOM",
		"Ukrainian Radio Systems"
	},

	/* Kyivstar GSM JSC */
	{
		255, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "UA-KYIVSTAR",
		"Kyivstar GSM JSC"
	},

	/* Kyivstar GSM JSC */
	{
		255, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "UA-KYIVSTAR",
		"Kyivstar GSM JSC"
	},

	/* Golden Telecom LLC */
	{
		255, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "UA-GT",
		"Golden Telecom LLC"
	},

	/* Astelit LLC */
	{255, 6, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "life:)", "life:)"},

	/*****************
	 **** Belarus ****
	 *****************/

	/* Mobile Digital Communications */
	{
		257, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BY VELCOM",
		"Mobile Digital Communications"
	},

	/* Mobile Digital Communications */
	{
		257, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "BY VELCOM",
		"Mobile Digital Communications"
	},

	/* JLLC Mobile TeleSystems  */
	{
		257, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MTS BY",
		"JLLC Mobile TeleSystems"
	},

	/* JLLC Mobile TeleSystems  */
	{
		257, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MTS BY",
		"JLLC Mobile TeleSystems"
	},

	/* Belarusskaya Set Telekommunkatsiy (BeST) */
	{257, 25, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BeST", "BeST"},

	/* Belarusskaya Set Telekommunkatsiy (BeST) */
	{257, 25, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "BeST", "BeST"},

	/*****************
	 **** Moldova ****
	 *****************/

	/* Voxtel S.A */
	{
		259, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MD VOXTEL",
		"VoXtel S.A"
	},

	/* Moldcell SA */
	{
		259, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MD MOLDCELL",
		"Moldcell SA"
	},

	/****************
	 **** Poland ****
	 ****************/

	/* Polkomtel S.A */
	{
		260, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Plus GSM",
		"Polkomtel S.A"
	},

	/* Polkomtel S.A */
	{
		260, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Plus GSM",
		"Polkomtel S.A"
	},

	/* Polkomtel S.A */
	{
		260, 1, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "Plus GSM",
		"Polkomtel S.A"
	},

	/* Polska Telefonia Cyfrowa */
	{
		260, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Era",
		"Polska Telefonia Cyfrowa"
	},

	/* Polska Telefonia Cyfrowa */
	{
		260, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Era",
		"Polska Telefonia Cyfrowa"
	},

	/* PTK Centertel */
	{
		260, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Orange PL",
		"PTK Centertel"
	},

	/* PTK Centertel */
	{
		260, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Orange PL",
		"PTK Centertel"
	},

	/* PTK Centertel */
	{
		260, 3, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "Orange PL",
		"PTK Centertel"
	},

	/*****************
	 **** Germany ****
	 *****************/

	/* T-Mobile Deutschland GmbH */
	{
		262, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "T-Mobile D",
		"T-Mobile Deutschland GmbH"
	},

	/* T-Mobile Deutschland GmbH */
	{
		262, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "T-Mobile D",
		"T-Mobile Deutschland GmbH"
	},

	/* Vodafone D2 GmbH */
	{
		262, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Vodafone.de",
		"Vodafone D2 GmbH"
	},

	/* Vodafone D2 GmbH */
	{
		262, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Vodafone.de",
		"Vodafone D2 GmbH"
	},

	/* E-Plus Mobilfunk GmbH */
	{
		262, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "E-Plus",
		"E-Plus Mobilfunk GmbH"
	},

	/* Viag Interkom */
	{
		262, 7, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "o2 - de",
		"Viag Interkom"
	},

	/* O2 (Germany) GmbH & Co. OHG */
	{
		262, 8, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "o2 - de",
		"O2 (Germany) GmbH & Co. OHG "
	},

	/* Mobilcom Multimedia GMBH */
	{
		262, 13, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "Mobilcom",
		"Mobilcom Multimedia GMBH"
	},

	/* Group 3G UMTS GmbH */
	{
		262, 14, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "Group3G",
		"Group 3G UMTS GmbH"
	},

	/*******************
	 **** Gibraltar ****
	 *******************/

	/* Gibraltar Telecoms Int'l */
	{
		266, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GIBTEL GSM",
		"Gibraltar Telecommunications Int. Ltd."
	},

	/******************
	 **** Portugal ****
	 ******************/

	/* Vodafone Telecel */
	{
		268, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone P",
		"Vodafone Telecel"
	},

	/* Vodafone Telecel */
	{
		268, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "vodafone P",
		"Vodafone Telecel"
	},

	/* Optimus Telecomunicacoes, S.A */
	{
		268, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "P OPTIMUS",
		"Optimus Telecomunicacoes, S.A"
	},

	/* Optimus Telecomunicacoes, S.A */
	{
		268, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "P OPTIMUS",
		"Optimus Telecomunicacoes, S.A"
	},

	/* Telecomunicacoes Moveis Nacionais S.A */
	{268, 6, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "P TMN", "TMN"},

	/* Telecomunicacoes Moveis Nacionais S.A */
	{268, 6, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "P TMN", "TMN"},

	/* TMN */
	{268, 6, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "P TMN", "TMN"},

	/********************
	 **** Luxembourg ****
	 ********************/
	/* P & T Luxembourg */
	{
		270, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "L LUXGSM",
		"P & T Luxembourg"
	},
	/* P & T Luxembourg */
	{
		270, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "L LUXGSM",
		"P & T Luxembourg"
	},

	/* Tango S.A */
	{
		270, 77, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "L Tango",
		"Tango S.A"
	},

	/* Tango S.A */
	{
		270, 77, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "L Tango",
		"Tango S.A"
	},

	/* VOXmobile S.A. */
	{
		270, 99, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "VOX.LU",
		"VOXmobile"
	},

	/* VOXmobile S.A. */
	{270, 99, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "VOX.LU", "VOXmobile"},

	/*****************
	 **** Ireland ****
	 *****************/

	/* Vodafone Ireland Plc */
	{
		272, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone IE",
		"Vodafone Ireland Plc"
	},

	/* Vodafone Ireland Ltd. */
	{
		272, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "vodafone IE",
		"vodafone"
	},

	/* Digifone MM02 Ltd */
	{
		272, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "O2 - IRL",
		"Digifone MM02 Ltd"
	},

	/* Digifone MM02 Ltd */
	{
		272, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "O2 - IRL",
		"Digifone MM02 Ltd"
	},

	/* Meteor Mobile Telecommunications Limited */
	{
		272, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IRL - METEOR",
		"Meteor Mobile Telecommunications Limited"
	},

	/* Meteor Mobile Telecommunications Limited */
	{
		272, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IRL - METEOR",
		"Meteor Mobile Telecommunications Limited"
	},

	/* Hutchison SETTING_PREFERRED_NETWORK_TYPE_UMTS Ireland limited */
	{
		272, 5, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "3 IRL",
		"Hutchison SETTING_PREFERRED_NETWORK_TYPE_UMTS Ireland"
	},

	/*****************
	 **** Iceland ****
	 *****************/

	/* Iceland Telecom Ltd */
	{
		274, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IS SIMINN",
		"Iceland Telecom Ltd"
	},

	/* Iceland Telecom Ltd */
	{
		274, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IS SIMINN",
		"Iceland Telecom Ltd"
	},

	/* Og fjarskipti hf */
	{
		274, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Og Vodafone",
		"Og fjarskipti hf"
	},

	/* Og fjarskipti hf */
	{
		274, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Og Vodafone",
		"Og fjarskipti hf"
	},

	/* Og fjarskipti hf */
	{
		274, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Og Vodafone",
		"Og fjarskipti hf"
	},

	/* Og fjarskipti hf */
	{
		274, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Og Vodafone",
		"Og fjarskipti hf"
	},

	/* IMC Island ehf */
	{
		274, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Viking",
		"IMC Island ehf"
	},

	/*****************
	 **** Albania ****
	 *****************/
	/* Albanian Mobile Communications */
	{
		276, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AMC - AL",
		"Albanian Mobile Communications"
	},

	/* Vodafone Albania */
	{
		276, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone AL",
		"Vodafone Albania"
	},

	/* Vodafone Albania */
	{
		276, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "vodafone AL",
		"Vodafone Albania"
	},

	/***************
	 **** Malta ****
	 ***************/

	/* Vodafone Malta Limited */
	{
		278, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone MT",
		"Vodafone Malta Limited"
	},

	/* Mobisle Communications Limited */
	{
		278, 21, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "gomobile",
		"Mobisle Communications Limited"
	},

	/****************
	 **** Cyprus ****
	 ****************/

	/* Cyprus Telecommunications Auth */
	{
		280, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CY CYTAGSM",
		"Cyprus Telecommunications Auth"
	},

	/* Cyprus Telecommunications Auth */
	{
		280, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "CY CYTAGSM",
		"Cyprus Telecommunications Auth"
	},

	/* Areeba LTD */
	{
		280, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "areeba",
		"Areeba LTD"
	},

	/* Areeba LTD */
	{
		280, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "areeba",
		"Areeba LTD"
	},

	/*****************
	 **** Georgia ****
	 *****************/

	/* Geocell Limited */
	{
		282, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GEO-GEOCELL",
		"Geocell Limited"
	},

	/* Geocell Limited */
	{
		282, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "GEO-GEOCELL",
		"Geocell Limited"
	},

	/* Magti GSM */
	{
		282, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MAGTI-GSM-GEO",
		"Magti GSM"
	},

	/* Magti GSM */
	{
		282, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MAGTI-GSM-GEO",
		"Magti GSM"
	},

	/*****************
	 **** Armenia ****
	 *****************/

	/* ArmenTel */
	{
		283, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RA-ARMGSM",
		"ArmenTel"
	},

	/* Karabakh Telecom */
	{
		283, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RA 04",
		"Karabakh Telecom"
	},

	/* K Telecom CJSC */
	{283, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RA 05", "VivaCell"},

	/* K Telecom CJSC */
	{283, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "RA 05", "VivaCell"},

	/******************
	 **** Bulgaria ****
	 ******************/

	/* MobilTEL AD */
	{
		284, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "M-TEL GSM BG",
		"MobilTEL AD"
	},

	/* Mobiltel EAD */
	{284, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "M-Tel", "M-Tel BG"},

	/* Cosmo Bulgaria Mobile EAD */
	{
		284, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BG GLOBUL",
		"Cosmo Bulgaria Mobile EAD"
	},

	/* Cosmo Bulgaria Mobile EAD */
	{
		284, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "BG GLOBUL",
		"Cosmo Bulgaria Mobile EAD"
	},

	/****************
	 **** Turkey ****
	 ****************/

	/* Turkcell */
	{
		286, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TR TURKCELL",
		"Turkcell Iletisim Hizmetleri A.S."
	},

	/* TELSIM Mobil Telekomuniksyon Hiz.A.S */
	{
		286, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TR TELSIM",
		"TELSIM Mobil Telekomuniksyon Hiz.A.S"
	},

	/* IS-TIM Telekomunikasyon Hizmetleri Anonim Sirketi */
	{
		286, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TR ARIA",
		"IS-TIM Telekomunikasyon Hizmetleri Anonim Sirketi"
	},

	/* Turk Telekomunikasyon A.S. */
	{
		286, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TR AYCELL",
		"AYCELL Haberlesme ve Pazarlama Hizmetleri A.S."
	},

	/***********************
	 **** Faroe Islands ****
	 ***********************/

	/* Faroese Telecom GSM 900 */
	{
		288, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "FROFT",
		"Faroese Telecom GSM 900"
	},

	/* P/F Kall (Kall Telecom) */
	{
		288, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "KALL",
		"P/F Kall (Kall Telecom)"
	},

	/*******************
	 **** Greenland ****
	 *******************/

	/* Tele Greenland A/S */
	{
		290, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TELE Greenland",
		"Tele Greenland A/S"
	},

	/******************
	 **** Slovenia ****
	 ******************/

	/* SI.Mobil d.d */
	{
		293, 40, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SI.MOBIL",
		"SI.Mobil d.d"
	},

	/* SI.Mobil d.d */
	{
		293, 40, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SI.MOBIL",
		"SI.Mobil d.d"
	},

	/* Mobitel d.d */
	{
		293, 41, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MOBITEL",
		"Mobitel D.D."
	},

	/* Mobitel d.d */
	{
		293, 41, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MOBITEL",
		"Mobitel D.D."
	},

	/* Western Wireless International d.o.o. */
	{
		293, 70, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SI VEGA 070",
		"Western Wireless International d.o.o."
	},

	/*******************
	 **** Macedonia ****
	 *******************/

	/* MOBIMAK AD */
	{
		294, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MKD-MOBIMAK",
		"MOBIMAK AD"
	},

	/* MTS Uslugi na mobilna telefonija AD - Skopje */
	{
		294, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MKD COSMOFON",
		"MTS Uslugi na mobilna telefonija AD - Skopje"
	},

	/***********************
	 **** Liechtenstein ****
	 ***********************/

	/* Swisscom Mobile Ltd (Liechtenstein) */
	{
		295, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "FL GSM",
		"Swisscom Mobile Ltd (Liechtenstein)"
	},

	/* Swisscom Mobile Ltd (Liechtenstein) */
	{
		295, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "FL GSM",
		"Swisscom Mobile Ltd (Liechtenstein)"
	},

	/* Orange (Liechtenstein) AG */
	{
		295, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Orange FL",
		"Orange (Liechtenstein) AG"
	},

	/* Mobilkom (Liechtenstein) AG */
	{
		295, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "FL1",
		"Mobilkom (Liechtenstein) AG"
	},

	/* Tele 2 Aktiengesellschaft */
	{
		295, 77, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LI TANGO",
		"Tele 2 Aktiengesellschaft"
	},

	/****************
	 **** Canada ****
	 ****************/

	/* Microcell Telecom Inc */
	{
		302, 37, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "MICROCELL",
		"Microcell Telecom Inc"
	},

	/* Fido */
	{302, 370, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Fido", "Fido"},

	/* Rogers Wireless */
	{
		302, 720, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Rogers Wireless",
		"Rogers Wireless"
	},

	/* SPM Telecom  */
	{308, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AMERIS", "AMERIS"},

	/**************************
	 **** Papua New Guinea ****
	 **************************/

	/* Pacific Mobile Comms */
	{
		310, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Pacific",
		"Pacific Mobile Comms"
	},

	/****************
	 **** U.S.A. ****
	 ****************/

	/* PA Cellnet */

	/*
	M625000001458
	"VSTREAM", "Voicestream Wireless Corporation"
	changed to
	"T-Mobile", "T-Mobile USA, Inc"
	--> MOHAN On recommendation from Mr. KIm Kiyong from USA during his testing. D500 shows T-Mobile for all VSTREAM
	*/
	{310, 10, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "PA Cellnet", "PA Cellnet"},	/* M625000000623 */

	/* Third Kentucky Cellular Corporation */
	{
		310, 11, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "WTTCKy",
		"Third Kentucky Cellular Corporation"
	},

	/* Cingular Wireless */
	{
		310, 15, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/* Voicestream Wireless Corporation */
	{
		310, 16, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* Cingular Wireless */
	{
		310, 17, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/* Dutch Harbor */
	{
		310, 19, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Dutch Harbor",
		"Dutch Harbor"
	},

	/* Sprint */
	{310, 20, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Sprint", "Sprint"},

	/* Union */
	{310, 20, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Union", "Union"},

	/* Union Telephone Company */
	{
		310, 20, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900,
		"US - Union Telephone", "Union Telephone Company"
	},

	/* Voicestream Wireless Corporation */
	{
		310, 20, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Union Telephone Company */
	{
		310, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_850,
		"US - Union Telephone", "Union Telephone Company"
	},
#else

	/* M625000002007 - Moved from Top to here, to maintain Proper MNC order inorder to avoid displaying of MCC and MNC */

	/* Sprint */
	{310, 20, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Sprint", "Sprint"},	/* M625000002007 */

	/* Union */
	{310, 20, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Union", "Union"},	/* M625000002007 */
#endif

	/* Voicestream Wireless Corporation */
	{
		310, 21, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* Voicestream Wireless Corporation */
	{
		310, 22, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* Voicestream Wireless Corporation */
	{
		310, 23, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* Voicestream Wireless Corporation */
	{
		310, 24, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* Voicestream Wireless Corporation */
	{
		310, 25, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* Voicestream Wireless Corporation */
	{
		310, 26, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* Powertel PCS Inc */
	{
		310, 27, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Powertel",
		"Powertel PCS Inc"
	},

	/* Voicestream Wireless Corporation */
	{
		310, 27, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* Centennial Wireless */
	{
		310, 30, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900,
		"Centennial Wireless", "Centennial Wireless"
	},
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Centennial Communications */
	{
		310, 30, SETTING_PREFERRED_NETWORK_TYPE_GSM_850,
		"Centennial Communications", "Centennial Communications"
	},
#endif

	/* Voicestream Wireless Corporation */
	{
		310, 31, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* High Plains/Midwest LLC */
	{
		310, 34, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "WestLink",
		"High Plains/Midwest LLC"
	},

	/* Cingular Wireless */
	{310, 38, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular", "Cingular Wireless"},	/*M627500000468 */

	/* Cellular One */
	{310, 40, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cellular One", "Cellular One"},	/* M625000000623 */

	/* Raju:Cingular Wireless */

	/* Added new entry for the cingular */
	{310, 41, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular", "Cingular Wireless"},	/*M627500000468 */

	/* TMP Corp */
	{310, 46, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "TMP", "TMP Corp"},

	/* DIGICEL */
	{310, 50, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "DIGICEL", "DIGICEL"},	/* M625000000623 */

	/* Key Communications, LLC */
	{
		310, 53, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "WVW",
		"Key Communications, LLC"
	},

	/* PCS One Inc. */
	{
		310, 58, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "VSTREAM",
		"PCS One Inc."
	},

	/* Choice Wireless L.C. */
	{
		310, 63, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Choice",
		"Choice Wireless L.C."
	},

	/* Airadigm Communications */
	{
		310, 64, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "ARDGMC",
		"Airadigm Communications"
	},
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Voicestream Wireless Corporation */
	{
		310, 70, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "T-Mobile",
		"T-Mobile USA, Inc"
	},
#endif

	/* Corr Wireless */
	{310, 80, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Corr Wireless", "Corr Wireless"},	/* M625000000623 */

	/* Plateau Wireless */
	{310, 100, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Plateau Wireless", "Plateau Wireless"},	/* M625000000623 */
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* E.N.M.R. Telephone Cooperative */
	{
		310, 100, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "US PLATEAU",
		"E.N.M.R. Telephone Cooperative"
	},

	/* New Mexico RSA 4 East Ltd Partnership */
	{
		310, 100, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "US PLATEAU",
		"New Mexico RSA 4 East Ltd Partnership"
	},

	/* Texas RSA 3 Ltd Partnership */
	{
		310, 100, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "US PLATEAU",
		"Texas RSA 3 Ltd Partnership"
	},
#endif

	/* WTTCKy */
	{310, 110, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "WTTCKy", "WTTCKy"},	/* M625000000623 */

	/* Cingular Wireless */
	{
		310, 150, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/* T-Mobile USA, Inc */
	{310, 160, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile", "T-Mobile USA, Inc"},	/* M625000000623 */

	/* Cingular Wireless */
	{
		310, 170, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/* Cingular Wireless */
	{310, 180, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular", "Cingular Wireless"},	/* M625000000623 */

	/* West Central */
	{310, 180, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "West Central", "West Central"},	/* M625000000623 */

	/* Alaska Wireless */
	{310, 190, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Alaska Wireless", "Alaska Wireless"},	/* M625000000623 */

	/* Dutch Harbor */
	{310, 190, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Dutch Harbor", "Dutch Harbor"},	/* M625000000623 */

	/* Alaska  Wireless Communications, LLC */
#ifdef FEATURE_SAMSUNG_BAND_GSM_850
	{
		310, 190, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "USA Dutch Harbor",
		"Alaska  Wireless Communications, LLC"
	},
#endif

	/* T-Mobile USA, Inc */
	{
		310, 200, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* T-Mobile USA, Inc */
	{
		310, 210, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* T-Mobile USA, Inc */
	{
		310, 220, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* T-Mobile USA, Inc */
	{
		310, 230, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* T-Mobile USA, Inc */
	{
		310, 240, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* T-Mobile USA, Inc */
	{
		310, 250, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* T-Mobile USA, Inc */
	{
		310, 260, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* T-Mobile USA, Inc */
	{
		310, 270, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* T-Mobile USA, Inc */
	{310, 310, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile", "T-Mobile USA, Inc"},	/* M625000000623 */

	/* Voicestream Wireless Corporation */
	{
		310, 310, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* Farmers Cellular Telephone Inc */
	{
		310, 311, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "FARMERS",
		"Farmers Cellular Telephone Inc"
	},
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Farmers Cellular Telephone Inc */
	{
		310, 311, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "FARMERS",
		"Farmers Cellular Telephone Inc"
	},

	/* Smith Bagley, Inc. */
	{
		310, 320, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "USA - CellularOne",
		"Smith Bagley, Inc."
	},
#endif

	/* Smith Bagley, Inc.*/
	{
		310, 320, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "USA - CellularOne",
		"Smith Bagley, Inc."
	},

	/* WestLink Comm */
	{310, 340, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "WestLink Comm", "WestLink Comm"},	/* M625000000623 */

	/* High Plains/Midwest LLC */
	{
		310, 340, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "USA 340",
		"High Plains/Midwest LLC"
	},

	/* WestLink Comm */
	{310, 350, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "WestLink Comm", "WestLink Comm"},	/* M625000000623 */

	/* Carolina Phone */
	{310, 350, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Carolina Phone", "Carolina Phone"},	/* M625000000623 */
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Cingular Wireless */
	{310, 380, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "Cingular", "Cingular Wireless"},	/* M627500000468 */
#endif

	/* Cingular Wireless */
	{310, 380, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular", "Cingular Wireless"},	/* M627500000468 */

	/* Yorkville */
	{310, 390, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Yorkville", "Yorkville"},	/* M625000000623 */

	/* Cingular Wireless */
	{310, 410, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular", "Cingular Wireless"},	/* M625000000623 */

	/* Cincinnati Bell Wireless */
	{310, 420, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cincinnati Bell Wireless", "Cincinnati Bell Wireless"},	/* M625000000623 */

	/* Viaero Wireless */
	{310, 450, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Viaero Wireless", "Viaero Wireless"},	/* M625000000623 */
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* North East Colorado Cellular, Inc (NECCI) */
	{
		310, 450, SETTING_PREFERRED_NETWORK_TYPE_GSM_850,
		"Cell One of NE Colorado",
		"North East Colorado Cellular, Inc (NECCI)"
	},
#endif

	/* USA ONELINK */
	{310, 460, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "USA ONELINK", "USA ONELINK"},	/* M625000000623 */

	/* SunCom */
	{310, 490, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "SunCom", "SunCom"},	/* M625000000623 */
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Public Service Cellular, Inc */
	{
		310, 500, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "PSC Wireless",
		"Public Service Cellular, Inc"
	},
#endif

	/* West Virginia Wireless */
	{
		310, 530, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900,
		"West Virginia Wireless", "West Virginia Wireless"
	},

	/* Key Communications, LLC */
	{
		310, 530, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "USA-WVA WIRELESS",
		"Key Communications, LLC"
	},

	/* Cellular One DCS */
	{310, 560, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cellular One DCS", "Cellular One DCS"},	/* M625000000623 */
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Dobson Communications Corporation */
	{
		310, 560, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "Cellular One DCS",
		"Dobson Communications Corporation"
	},
#endif

	/* T-Mobile USA, Inc */
	{310, 580, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile", "T-Mobile USA, Inc"},	/* M625000000623 */
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* WWC Holding Co., Inc */
	{
		310, 590, SETTING_PREFERRED_NETWORK_TYPE_GSM_850,
		"USA - Extended Area", "WWC Holding Co., Inc"
	},
#endif

	/* Epic Touch */
	{310, 610, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Epic Touch", "Epic Touch"},	/* M625000000623 */

	/* Choice Wireless L.C. */
	{
		310, 630, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Choice",
		"Choice Wireless L.C."
	},

	/* AmeriLink PCS */
	{310, 630, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "AmeriLink PCS", "AmeriLink PCS"},	/* M625000000623 */

	/* Einstein PCS */
	{310, 640, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Einstein PCS", "Einstein PCS"},	/* M625000000623 */

	/* Airadigm Communications */
	{
		310, 640, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "USA AE Airadigm",
		"Airadigm Communications"
	},

	/* T-Mobile USA, Inc */
	{
		310, 660, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile",
		"T-Mobile USA, Inc"
	},

	/* W 2000 PCS */
	{310, 670, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "W 2000 PCS", "W 2000 PCS"},	/* M625000000623 */

	/* NPI Wireless */
	{310, 680, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "NPI Wireless", "NPI Wireless"},	/* M625000000623 */

	/* Immix Wireless */
	{310, 690, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Immix Wireless", "Immix Wireless"},	/* M625000000623 */

	/* Conestoga */
	{310, 690, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Conestoga", "Conestoga"},	/* M625000000623 */

	/* HIGHLAND */
	{310, 700, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "HIGHLAND", "HIGHLAND"},	/* M625000000623 */

	/* Rogers */
	{310, 720, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Rogers", "Rogers"},	/* M625000000623 */

	/* Telemetrix */
	{310, 740, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Telemetrix", "Telemetrix"},	/* M625000000623 */

	/* PTSI */
	{310, 760, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "PTSI", "PTSI"},	/* M625000000623 */

	/* i wireless */
	{310, 770, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "i wireless", "i wireless"},	/* M625000000623 */

	/* AirLink PCS */
	{310, 780, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "AirLink PCS", "AirLink PCS"},	/* M625000000623 */

	/* PinPoint */
	{310, 790, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "PinPoint", "PinPoint"},	/* M625000000623 */

	/* T-Mobile USA, Inc */
	{310, 800, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "T-Mobile", "T-Mobile USA, Inc"},	/* M625000000623 */

	/* Advantage Cellular Systems Inc */
	{
		310, 880, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "USAACSI",
		"Advantage Cellular Systems Inc"
	},

	/* Southern Illinois RSA Partnership */
	{
		310, 910, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "USAFC",
		"Southern Illinois RSA Partnership"
	},

	/* Lynn County Cellular Limited Partnership */
	{
		310, 940, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "Digital Cellular",
		"Lynn County Cellular Limited Partnership"
	},

	/* Texas RSA 8 West Limited */
	{
		310, 940, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "Digital Cellular",
		"Texas RSA 8 West Limited"
	},

	/* XIT Cellular */
	{
		310, 950, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "USA XIT Cellular",
		"XIT Cellular"
	},

	/* Cingular Wireless */
	{310, 980, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular", "Cingular Wireless"},	/* M625000000623 */

	/* Mid-Tex Cellular, Ltd. */
	{
		311, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_850,
		"USA Mid-Tex Cellular, Ltd", "Mid-Tex Cellular, Ltd."
	},

	/* Wilkes Cellular Inc */
	{
		311, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "WILKES",
		"Via Wireless"
	},

	/* Wilkes Cellular Inc */
	{
		311, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "WILKES",
		"Via Wireless"
	},

	/* "Indigo Wireless, Inc" */
	{
		311, 30, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Indigo",
		"Indigo Wireless"
	},

	/* Easterbrooke Cellular Corporation */
	{
		311, 70, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "EASTER",
		"Easterbrooke"
	},

	/* Pine Telephone Company */
	{
		311, 80, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "Pine Cellular",
		"Pine Telephone Company"
	},

	/* Long Lines Wireless LLC */
	{
		311, 90, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "SXLP",
		"Long Lines Wireless"
	},

	/* "High Plains Wireless, L.P" */
	{
		311, 110, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "HPW",
		"High Plains Wireless"
	},

	/* "Amarillo License, L.P" */
	{
		311, 130, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C1AMARIL",
		"Cellular One of Amarillo"
	},

	/* Petrocom LLC */
	{
		311, 170, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "PetroCom",
		"PetroCom"
	},

	/* "Cellular Properties, Inc  " */
	{
		311, 190, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C1ECI",
		"Cellular One of East Central Illinois"
	},

	/* Farmers Cellular Telephone Inc */
	{
		311, 210, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "FARMERS",
		"Farmers"
	},

	/* Farmers Cellular Telephone Inc */
	{
		311, 210, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "FARMERS",
		"Farmers"
	},

	/* Wave Runner LLC (Guam) */
	{
		311, 250, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "iCAN_GSM",
		"i CAN_GSM"
	},

	/* "Lamar County Cellular, Inc" */
	{
		311, 310, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "LamarCel",
		"Lamar County Cellular"
	},

	/* Sprocket */
	{311, 140, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Sprocket", "Sprocket"},	/* M625000000623 */

	/* Cingular Wireless */
	{311, 180, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular", "Cingular Wireless"},	/* M625000000623 */

	/****************
	 **** Mexico ****
	 ****************/

	/* Pegaso Comunicaciones y Sistemas, S.A. De C.V */
	{
		334, 3, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "MX MOVISTAR GSM",
		"Pegaso Comunicaciones y Sistemas, S.A. De C.V"
	},

	/* Telcel GSM */
	{334, 20, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Telcel GSM", "Telcel GSM"},	/* M625000000623 */

	/*****************
	 **** Jamaica ****
	 *****************/

	/* Mossel Limited T/A Digicel */
	{
		338, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "JM DIGICEL",
		"Mossel Limited T/A Digicel"
	},

	/* Cable & Wireless Jamaica Limited */
	{
		338, 180, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "C&W",
		"Cable & Wireless Jamaica Limited"
	},

	/****************************
	 **** French West Indies ****
	 ****************************/

	/* ORANGE CARAIBE */
	{340, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "F-Orange", "Orange Caraibe"},	/* M627500002609 */

	/* Outremer Telecom */
	{340, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ONLY", "Outremer"},

	/* Outremer Telecom */
	{340, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "ONLY", "Outremer"},

	/* Dauphin Telecom */
	{
		340, 8, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AMIGO",
		"Dauphin Telecom"
	},

	/* Dauphin Telecom */
	{
		340, 8, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AMIGO",
		"Dauphin Telecom"
	},

	/* Bouygues Telecom Caraibe */
	{
		340, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BOUYGTEL-C",
		"Bouygues Telecom Caraibe"
	},

	/* Bouygues Telecom Caraibe */
	{
		340, 20, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "BOUYGTEL-C",
		"Bouygues Telecom Caraibe"
	},

	/*****************
	 **** Barbados ****
	 *****************/

	/* Digicel (Barbados) Limited */
	{
		342, 50, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "JM Digicel",
		"Digicel (Barbados) Limited"
	},

	/* Digicel (Barbados) Limited */
	{
		342, 50, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "JM Digicel",
		"Digicel (Barbados) Limited"
	},

	/* Cable & Wireless Barbados Ltd. */
	{
		342, 600, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "C&W",
		"Cable & Wireless Barbados Ltd."
	},

	/* Digicel (Barbados) Limited */
	{
		342, 750, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "DIGICEL",
		"Digicel"
	},

	/* Digicel (Barbados) Limited */
	{
		342, 750, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "DIGICEL",
		"Digicel"
	},

	/* Cellular Communications Barbados */
	{
		342, 810, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Cingular",
		"Cingular Wireless"
	},

	/* Cellular Communications Barbados */
	{
		342, 810, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/*****************
	 **** Antigua ****
	 *****************/

	/* Antigua Public Utilities Authority-APUA */
	{
		344, 30, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "APUA PCS ANTIGUA",
		"Antigua Public Utilities Authority-APUA"
	},
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Cable & Wireless Caribbean Cellular (Antigua) Limited */
	{
		344, 920, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C&W",
		"Cable & Wireless Caribbean Cellular (Antigua) Limited"
	},
#endif				/* FEATURE_SAMSUNG_BAND_GSM_850 */

	/* Antigua Wireless Ventures Limited */
	{
		344, 930, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Cingular",
		"Cingular Wireless"
	},

	/*********************
	 **** Cayman Islands ****
	 *********************/

	/* Cable & Wireless (Cayman Islands) Limited */
	{
		346, 140, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "C&W",
		"Cable & Wireless (Cayman Islands) Limited"
	},
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Cable & Wireless (Cayman Islands) Limited */
	{
		346, 140, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C&W",
		"Cable & Wireless (Cayman Islands) Limited"
	},
#endif				/* FEATURE_SAMSUNG_BAND_GSM_850 */

	/* Caribbean Cellular Telephone  */
	{348, 570, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CCTBVI", "CCT"},

	/* Caribbean Cellular Telephone  */
	{348, 570, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "CCTBVI", "CCT"},

	/*****************
	 **** Bermuda ****
	 *****************/

	/* Telecommunications (Bermuda & West Indies) Ltd */
	{
		350, 1, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "TELECOM BDA",
		"Telecommunications (Bermuda & West Indies) Ltd"
	},

	/* Mobility Limited */
	{
		350, 2, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "BTC MOBILITY LTD.",
		"Mobility Limited"
	},

	/* Telecommunications (Bermuda & West Indies) Ltd */
	{
		350, 10, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/***************
	 **** Grenada ****
	 ***************/

	/* Digicel Grenada Ltd. */
	{352, 30, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "DIGICEL", "Digicel"},

	/* Digicel Grenada Ltd. */
	{
		352, 30, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "DIGICEL",
		"Digicel"
	},

	/* Grenada Wireless Holdings Ltd */
	{
		352, 30, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Cingular",
		"Cingular Wireless"
	},

	/* Grenada Wireless Holdings Ltd */
	{
		352, 30, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/* Cable & Wireless Caribbean Cellular (Grenada) Limited */
	{
		352, 110, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C&W",
		"Cable & Wireless Caribbean Cellular (Grenada) Limited"
	},

	/* Cable & Wireless West Indies Ltd (Montserrat) */
	{
		354, 860, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C&W",
		"Cable & Wireless West Indies (Montserrat)"
	},

	/*********************
	 **** St Kitts & Nevis ****
	 *********************/

	/* Cable & Wireless St Kitts & Nevis Limited */
	{
		356, 110, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C&W",
		"Cable & Wireless St Kitts & Nevis Limited"
	},

	/* Cable & Wireless St Kitts & Nevis Limited */
	{
		356, 110, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "C&W",
		"Cable & Wireless St Kitts & Nevis Limited"
	},

	/***************
	 **** St Lucia ****
	 ***************/

	/* Wireless Ventures (St. Lucia) Limited */
	{
		358, 30, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Cingular",
		"Cingular Wireless"
	},

	/* Wireless Ventures (St. Lucia) Limited */
	{
		358, 30, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/* Digicel (St Lucia) Limited */
	{
		358, 50, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "DIGICEL",
		"Digicel (St Lucia) Limited"
	},

	/* Digicel (St Lucia) Limited */
	{
		358, 50, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "DIGICEL",
		"Digicel (St Lucia) Limited"
	},

	/* Digicel (St Lucia) Limited */
	{
		358, 50, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "DIGICEL",
		"Digicel (St Lucia) Limited"
	},

	/* Cable & Wireless Caribbean Cellular (St Lucia) Limited */
	{
		358, 110, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C&W",
		"Cable & Wireless"
	},

	/*********************************
	 **** St. Vincent & the Grenadines ****
	 ********************************/

	/* Wireless Ventures (St. Vincent) Limited */
	{
		360, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Cingular",
		"Cingular Wireless"
	},

	/* Wireless Ventures (St. Vincent) Limited */
	{
		360, 10, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/* Digicel (St. Vincent and the Grenadines) Limited */
	{
		360, 70, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "DIGICEL",
		"Digicel (St. Vincent and Grenadines) Limited"
	},

	/* Digicel (St. Vincent and the Grenadines) Limited */
	{
		360, 70, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "DIGICEL",
		"Digicel (St. Vincent and Grenadines) Limited"
	},

	/* Cable & Wireless Caribbean Cellular (St. Vincent & the Grenadines) Ltd */
	{
		360, 110, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C&W",
		"Cable & Wireless Caribbean Cellular (St. Vincent & the Grenadines) Ltd"
	},

	/******************************
	 **** Netherlands Antilles ****
	 ******************************/

	/* Telcell N.V. */
	{
		362, 51, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Telcell GSM",
		"Telcell N.V."
	},

	/* Curacao Telecom N.V. */
	{
		362, 69, SETTING_PREFERRED_NETWORK_TYPE_GSM_900,
		"ANT CURACAO TELECOM GSM", "Curacao Telecom N.V."
	},

	/* Curacao Telecom N.V. */
	{
		362, 69, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800,
		"ANT CURACAO TELECOM GSM", "Curacao Telecom N.V."
	},

	/* Setel NV */
	{362, 91, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Setel", "Setel NV"},

	/* Communications Systems Curacao N.V. */
	{
		362, 630, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/* Setel NV */
	{
		362, 951, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CHIPPIE",
		"UTS Wireless Curacao"
	},

	/**************
	 **************
	 **************/

	/* SETAR (Servicio di Telecomunicacion di Aruba) */
	{
		363, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SETAR GSM",
		"SETAR (Servicio di Telecomunicacion di Aruba)"
	},

	/* SETAR (Servicio di Telecomunicacion di Aruba) */
	{
		363, 1, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "SETAR GSM",
		"SETAR (Servicio di Telecomunicacion di Aruba)"
	},

	/* New Millenium Telecom Services (NMTS) */
	{363, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "DIGICEL", "Digicel"},

	/* New Millenium Telecom Services (NMTS) */
	{
		363, 20, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "DIGICEL",
		"Digicel"
	},

	/**************
	 **** BAHMAS ***
	 **************/

	/* The Bahamas Telecommunications Company Ltd */
	{
		364, 39, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "BaTelCell",
		"The Bahamas Telecommunications Company Ltd"
	},

	/***************
	 **** Anguilla ****
	 ***************/
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Cable & Wireless (West Indies) Ltd. Anguilla */
	{
		365, 840, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C&W",
		"Cable & Wireless (West Indies) Ltd. Anguilla"
	},
#endif				/* FEATURE_SAMSUNG_BAND_GSM_850 */

	/****************
	 **** Dominica ****
	 ****************/
	/* Wireless Ventures (Dominica) Ltd. */
	{
		366, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Cingular",
		"Cingular Wireless"
	},

	/* Wireless Ventures (Dominica) Ltd. */
	{
		366, 20, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Cingular",
		"Cingular Wireless"
	},

	/* Cable & Wireless Dominica Ltd. */
	{
		366, 110, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C&W",
		"Cable & Wireless Dominica Ltd."
	},

	/**************
	 **** Cuba ****
	 **************/

	/* "Empresa de Telecommunicaciones de Cuba, SA (ETECSA)" */
	{368, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "C_COM", "ETECSA"},

	/****************************
	 **** Dominican Republic ****
	 ****************************/

	/* Orange Dominicana S.A. */
	{
		370, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "orange",
		"Orange Dominicana S.A."
	},

	/* Orange Dominicana S.A. */
	{
		370, 1, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "orange",
		"Orange Dominicana S.A."
	},

	/* ORANGE */
	{370, 10, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "ORANGE", "ORANGE"},

	/* Communication Cellulaire d'Haiti SA */
	{372, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "VOILA", "Comcel "},

	/*****************************
	 **** Trinidad and Tobaga ****
	 *****************************/

	/* Telcommunications Services of Trinidad and Tobago Ltd */
	{
		374, 12, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TSTT",
		"Telcommunications Services of Trinidad and Tobago Ltd"
	},

	/**********************
	 **** Turks & Caicos ****
	 **********************/

	/* Cable & Wireless West Indies Ltd (Turks & Caicos) */
	{
		376, 350, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "C&W",
		"Cable & Wireless West Indies Ltd (Turks & Caicos)"
	},

	/********************
	 **** Azerbaijan ****
	 ********************/

	/* Azercell Telekom B.M. */
	{
		400, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AZE - AZERCELL GSM",
		"Azercell Telekom B.M."
	},

	/* Bakcell */
	{
		400, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BAKCELL GSM 2000",
		"J.V. Bakcell"
	},

	/********************
	 **** Kazakhstan ****
	 ********************/

	/* Kar-Tel Ltd */
	{
		401, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "KZ K-MOBILE",
		"Kar-Tel Ltd"
	},

	/* GSM Kazakhstan Ltd */
	{
		401, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "KZ KCELL",
		"GSM Kazakhstan Ltd"
	},

	/********************
	 **** *********** ****
	 ********************/

	/* B-Mobile */
	{
		402, 11, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BT B-Mobile",
		"B-Mobile"
	},

	/***************
	 **** India ****
	 ***************/

	/* Aircel Digilink India Ltd */
	{
		404, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Hutch",
		"Aircel Digilink India Ltd"
	},

	/* Bharti Mobile Ltd */
	{
		404, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AirTel",
		"Bharti Mobile Ltd"
	},

	/* Bharti Mobile Ltd */
	{
		404, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AirTel",
		"Bharti Mobile Ltd"
	},

	/* IDEA Cellular Limited */
	{
		404, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IDEA",
		"IDEA Cellular Limited"
	},

	/* Fascel Limited */
	{
		404, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Hutch",
		"Fascel Limited"
	},

	/* IDEA Cellular Limited */
	{
		404, 7, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IDEA",
		"IDEA Cellular Limited"
	},

	/* Reliance Telecom Private Ltd */
	{
		404, 9, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Reliance",
		"Reliance Telecom Private Ltd"
	},

	/* Bharti Mobile Ltd */
	{
		404, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AirTel",
		"Bharti Mobile Ltd"
	},

	/* Hutchison Essar Telecom Limited */
	{
		404, 11, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Hutch",
		"Hutchison Essar Telecom Limited"
	},

	/* Hutchison Essar Telecom Limited */
	{
		404, 11, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Hutch",
		"Hutchison Essar Telecom Limited"
	},

	/* Escotel Mobile Communications Ltd, Haryana */
	{
		404, 12, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "INDEH",
		"Escotel Mobile Communications"
	},

	/* Hutchison Essar South Limited */
	{
		404, 13, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Hutch",
		"Hutchison Essar South Limited"
	},

	/* Spice Communications Limited - Punjab */
	{
		404, 14, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "INA SPICE",
		"Spice Communications Limited"
	},

	/* Aircel Digilink India Ltd */
	{
		404, 15, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Hutch",
		"Aircel Digilink India Ltd"
	},

	/* Dishnet Wireless Limited */
	{404, 17, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRCEL", "Aircel"},

	/* Dishnet Wireless Limited */
	{404, 17, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AIRCEL", "Aircel"},

	/* Reliance Telecom Private Ltd */
	{
		404, 18, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Reliance",
		"Reliance Telecom Private Ltd"
	},

	/* Escotel Mobile Communications Ltd, Kerala */
	{
		404, 19, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "INDEK",
		"Escotel Mobile Communications"
	},

	/* Hutchison Essar Limited */
	{404, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Hutch", "HUTCH"},

	/* Hutchison Essar Limited */
	{404, 20, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Hutch", "HUTCH"},

	/* BPL Mobile Communications Ltd - Mumbai */
	{
		404, 21, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BPL MOBILE",
		"BPL Mobile Communications Ltd"
	},

	/* IDEA Cellular Limited - Maharashtra/Goa */
	{
		404, 22, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IDEA",
		"IDEA Cellular Limited"
	},

	/* IDEA Cellular Limited - Gujarat */
	{
		404, 24, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IDEA",
		"IDEA Cellular Limited"
	},

	/* Dishnet Wireless Limited */
	{404, 25, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRCEL", "Aircel"},

	/* Dishnet Wireless Limited */
	{404, 25, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AIRCEL", "Aircel"},

	/* BPL Cellular Limited - Maharshtra/Goa */
	{
		404, 27, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BPL MOBILE",
		"BPL Cellular Limited"
	},

	/* Dishnet Wireless Limited */
	{404, 28, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRCEL", "Aircel"},

	/* Dishnet Wireless Limited */
	{404, 28, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AIRCEL", "Aircel"},

	/* Dishnet Wireless Limited */
	{404, 29, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRCEL", "Aircel"},

	/* Dishnet Wireless Limited */
	{404, 29, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AIRCEL", "Aircel"},

	/* Hutchison Telecom East Limited */
	{
		404, 30, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "INA HUTCH",
		"Hutchison Telecom East Limited"
	},

	/* BHARTI MOBITEL LIMITED */
	{
		404, 31, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AirTel",
		"Bharti Cellular Ltd"
	},

	/* Dishnet Wireless Limited */
	{404, 33, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRCEL", "Aircel"},

	/* Dishnet Wireless Limited */
	{404, 33, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AIRCEL", "Aircel"},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 34, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Dishnet Wireless Limited */
	{404, 35, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRCEL", "Aircel"},

	/* Dishnet Wireless Limited */
	{404, 35, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AIRCEL", "Aircel"},

	/* Reliance Telecom Private Ltd */
	{
		404, 36, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Reliance",
		"Reliance Telecom Private Ltd"
	},

	/* Dishnet Wireless Limited */
	{404, 37, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRCEL", "Aircel"},

	/* Dishnet Wireless Limited */
	{404, 37, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AIRCEL", "Aircel"},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 38, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharti Cellular Ltd */
	{
		404, 40, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "INA AIRTEL",
		"Bharti Cellular Ltd"
	},

	/* RPG Cellular Services */
	{
		404, 41, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "INA RPG",
		"RPG Cellular Services"
	},

	/* Srinivas Cellcom Limited */
	{
		404, 42, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "INA AIRCEL",
		"Aircel Limited"
	},

	/* BPL Cellular Limited - Tamil Nadu/Pondicherry */
	{
		404, 43, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BPL MOBILE",
		"BPL Cellular Limited"
	},

	/* Spice Communications Limited - Karnataka */
	{
		404, 44, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "INA SPICE",
		"Spice Communications Limited"
	},

	/* Bharti Mobile Ltd - Karnataka */
	{
		404, 45, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AirTel",
		"Bharti Mobile Ltd"
	},

	/* BPL Cellular Limited - Kerala */
	{
		404, 46, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BPL MOBILE",
		"BPL Cellular Limited"
	},

	/* Bharti Mobile Ltd - Andhra Pradesh */
	{
		404, 49, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AirTel",
		"Bharti Cellular Ltd"
	},

	/* Reliance Telecom Private Ltd */
	{
		404, 50, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Reliance",
		"Reliance Telecom Private Ltd"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 51, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Reliance Telecom Private Ltd */
	{
		404, 52, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Reliance",
		"Reliance Telecom Private Ltd"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 53, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 54, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 55, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Escotel Mobile Communications Ltd, UP(W) */
	{
		404, 56, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "INDEU",
		"Escotel Mobile Communications"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 57, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 58, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 59, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Aircel Digilink India Ltd */
	{
		404, 60, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Hutch",
		"Aircel Digilink India Ltd"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 62, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 64, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 66, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Reliance Telecom Private Ltd */
	{
		404, 67, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Reliance",
		"Reliance Telecom Private Ltd"
	},

	/* Mahanagar Telephone Nigam Ltd */
	{
		404, 68, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IN-DOLPHIN",
		"Mahanagar Telephone Nigam Ltd"
	},

	/* Mahanagar Telephone Nigam Ltd */
	{
		404, 69, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IN-DOLPHIN",
		"Mahanagar Telephone Nigam Ltd"
	},

	/* Hexacom India Limited */
	{
		404, 70, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "INDH1",
		"Hexacom India Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 71, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 72, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 73, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 74, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 75, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 76, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 77, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* IDEA Cellular Limited */
	{
		404, 78, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IDEA",
		"IDEA Cellular Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 79, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 80, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Bharat Sanchar Nigam Limited */
	{
		404, 81, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BSNL MOBILE",
		"Bharat Sanchar Nigam Limited"
	},

	/* Hutchison Essar South Limited */
	{
		404, 84, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "HUTCH",
		"Hutchison Essar South Limited"
	},

	/* Reliance Telecom Private Ltd */
	{
		404, 85, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Reliance",
		"Reliance Telecom Private Ltd"
	},

	/* Hutchison Essar South Limited */
	{
		404, 86, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "HUTCH",
		"Hutchison Essar South Limited"
	},

	/* Bharti Cellular Ltd */
	{
		404, 90, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AirTel",
		"Bharti Cellular Ltd"
	},

	/* Bharti Cellular Ltd */
	{
		404, 92, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AirTel",
		"Bharti Cellular Ltd"
	},

	/* Bharti Cellular Ltd */
	{
		404, 93, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AirTel",
		"Bharti Cellular Ltd"
	},

	/* Bharti Cellular Ltd */
	{
		404, 94, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AirTel",
		"Bharti Cellular Ltd"
	},

	/* Bharti Cellular Ltd */
	{
		404, 95, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AirTel",
		"Bharti Cellular Ltd"
	},

	/* Bharti Cellular Ltd */
	{
		404, 96, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AirTel",
		"Bharti Cellular Ltd"
	},

	/* Bharti Cellular Ltd */
	{
		404, 97, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AirTel",
		"Bharti Cellular Ltd"
	},

	/* Bharti Cellular Ltd */
	{
		404, 98, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AirTel",
		"Bharti Cellular Ltd"
	},

	/********************
	 ***			   **
	  *******************/

	/* Bharti Televentures Limited */
	{405, 51, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRTEL", "AirTel"},

	/* Bharti Televentures Limited */
	{405, 52, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRTEL", "AirTel"},

	/* Bharti Televentures Limited */
	{405, 53, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRTEL", "AirTel"},

	/* Bharti Televentures Limited */
	{405, 54, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRTEL", "AirTel"},

	/* Bharti Televentures Limited */
	{405, 55, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRTEL", "AirTel"},

	/* Bharti Televentures Limited */
	{405, 56, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Airtel", "Airtel"},

	/* Hutchison Essar South Limited */
	{
		405, 66, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Hutch",
		"Hutch- UP West"
	},

	/* Hutchison Essar South Limited */
	{405, 67, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "HUTCH", "Hutch"},

	/******************
	 **** Pakistan ****
	 ******************/

	/* Mobilink */
	{
		410, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "PK-M-LINK",
		"Mobilink"
	},

	/* Pakistan Telecommunciation Mobile Ltd */
	{
		410, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "PK-UFONE",
		"Pakistan Telecommunciation Mobile Ltd"
	},

	/* Paktel Limited  */
	{410, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "PAKTEL", "PAKTEL"},

	/* Telenor Pakistan (Pvt) Ltd. */
	{
		410, 6, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TELENOR",
		"Telenor Pakistan (Pvt) Ltd."
	},

	/* Telenor Pakistan (Pvt) Ltd. */
	{
		410, 6, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TELENOR",
		"Telenor Pakistan (Pvt) Ltd."
	},

	/* Warid Telecom (PVT) Ltd */
	{
		410, 7, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "WaridTel",
		"Warid Telecom (PVT) Ltd"
	},

	/* Warid Telecom (PVT) Ltd */
	{
		410, 7, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "WaridTel",
		"Warid Telecom (PVT) Ltd"
	},

	/******************
	 **** Afghanistan ****
	 ******************/

	/* Telephone Systems International Inc */
	{
		412, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AF AWCC",
		"Telephone Systems International Inc"
	},

	/* Telephone Systems International Inc */
	{
		412, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AF AWCC",
		"Telephone Systems International Inc"
	},

	/* Telecom Development Company Afghanistan Ltd. */
	{
		412, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AF AWCC",
		"Telecom Development Company Afghanistan Ltd."
	},

	/*******************
	 **** Sri Lanka ****
	 *******************/

	/* Mobitel (Pvt) Limited */
	{
		413, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Mobitel",
		"Mobitel (Pvt) Limited"
	},

	/* MTN Networks (Pvt) Ltd */
	{
		413, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SRI DIALOG",
		"MTN Networks (Pvt) Ltd"
	},

	/* Celltel Lanka Limited */
	{
		413, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SRI CELLTEL",
		"Celltel Lanka Limited"
	},

	/* Hutchison Telecommunications Lanka (Pte) Limited */
	{
		413, 8, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Hutch",
		"Hutchison Telecommunications Lanka (Pvt) Limited"
	},

	/*****************
	 **** Myanmar ****
	 *****************/

	/* Myanmar Posts and Telecommunications */
	{
		414, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MM 900",
		"Myanmar Posts and Telecommunications"
	},

	/*****************
	 **** Lebanon ****
	 *****************/

	/* FTML Service Cellulaire */
	{
		415, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RL Cellis",
		"FTML Service Cellulaire"
	},

	/* LibanCell */
	{
		415, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RL LibanCell",
		"LibanCell"
	},

	/****************
	 **** Jordan ****
	 ****************/

	/* Jordan M.T.S */
	{
		416, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Fastlink",
		"Jordan Mobile Telephone Services (JMTS)"
	},

	/* Umniah Mobile Company */
	{416, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "UMNIAH", "Umniah"},

	/* MobileCom */
	{
		416, 77, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "JO MobCom",
		"Petra Jordanian Mobile Telecommunications Company(MobileCom)"
	},

	/***************
	 **** Syria ****
	 ***************/
	/* Syriatel Mobile Telecom SA */
	{
		417, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SYRIATEL",
		"Syriatel Mobile Telecom SA"
	},

	/* Syriatel Mobile Telecom SA */
	{
		417, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SYRIATEL",
		"Syriatel Mobile Telecom SA"
	},

	/* Spacetel Syria */
	{
		417, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "94 SYRIA",
		"Spacetel Syria"
	},

	/* Spacetel Syria */
	{
		417, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "94 SYRIA",
		"Spacetel Syria"
	},

	/* Syrian Telecom Est. */
	{
		417, 9, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SYR MOBILE SYR",
		"Syrian Telecom Est."
	},

	/* Syriatel Holdings S.A */
	{
		417, 93, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Syriatel",
		"Syriatel Holdings S.A"
	},

	/* Syriatel Holdings S.A */
	{
		417, 93, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Syriatel",
		"Syriatel Holdings S.A"
	},

	/****************
	 **** Iraq ****
	 ****************/

	/* Asia Cell Telecommunications Company Ltd */
	{
		418, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ASIACELL",
		"Asia Cell Telecommunications Company Ltd"
	},

	/* SanaTel */
	{418, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SanaTel", "SanaTel"},

	/* Asia Cell Telecommunications Company Ltd */
	{
		418, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ASIACELL",
		"Asia Cell Telecommunications Company Ltd"
	},

	/* SanaTel */
	{418, 8, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SanaTel", "SanaTel"},

	/* Atheer Telecom Iraq */
	{
		418, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Atheer Iraq",
		"Atheer Telecom Iraq"
	},

	/* Atheer Telecom Iraq */
	{
		418, 30, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IRAQNA",
		"Orascom Telecom Iraq Corporation"
	},

	/****************
	 **** Kuwait ****
	 ****************/

	/* Mobile Telecommunications Company */
	{
		419, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "KT MTCNet",
		"Mobile Telecommunications Company"
	},

	/* Mobile Telecommunications Company */
	{
		419, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "KT MTCNet",
		"Mobile Telecommunications Company"
	},

	/* National Mobile Telecommunications Co */
	{
		419, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "KT WATANIYA",
		"National Mobile Telecommunications Co"
	},

	/* National Mobile Telecommunications Co */
	{
		419, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "KT WATANIYA",
		"National Mobile Telecommunications Co"
	},

	/**********************
	 **** Saudi Arabia ****
	 **********************/

	/* Saudi Telecom Company */
	{
		420, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ALJAWAL",
		"Saudi Telecom Company"
	},

	/* Etihad Etisalat Company */
	{420, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Mobily", "Mobily"},

	/* Etihad Etisalat Company */
	{420, 3, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "Mobily", "Mobily"},

	/* Electronics App' Est. */
	{
		420, 7, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "EAE",
		"Electronics App' Est."
	},

	/***************
	 **** Yemen ****
	 ***************/

	/* Spacetel - Yemen */
	{
		421, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SabaFon",
		"Yemen Mobile Phone Company - Sabafon"
	},

	/* Spacetel - Yemen */
	{
		421, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SPACETEL",
		"Spacetel - Yemen"
	},

	/**************
	 **** Oman ****
	 **************/

	/* Oman Telecommunications Company */
	{
		422, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "OMAN MOBILE",
		"Oman Telecommunications Company"
	},

	/* Omani Qatari Telecommunications Company SAOC  */
	{422, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "nawras", "nawras"},

	/******************************
	 **** United Arab Emirates ****
	 ******************************/

	/* Emirates Telecom Corp-ETISALAT */
	{
		424, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "UAE ETISALAT",
		"Emirates Telecom Corp-ETISALAT"
	},

	/****************
	 **** Israel ****
	 ****************/

	/* Partner Communications Company Ltd */
	{
		425, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IL ORANGE",
		"Partner Communications Company Ltd"
	},

	/* Partner Communications Company Ltd */
	{
		425, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IL ORANGE",
		"Partner Communications Company Ltd"
	},

	/* Cellcom Israel Ltd */
	{
		425, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IL Cellcom",
		"Cellcom Israel Ltd"
	},

	/* Cellcom Israel Ltd */
	{
		425, 2, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "IL Cellcom",
		"Cellcom Israel Ltd"
	},

	/*******************************
	 **** Palestinian Authority ****
	 *******************************/

	/* Palestine Telecoms Co Plc */
	{
		425, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "JAWWAL-PALESTINE",
		"Palestine Telecoms Co Plc"
	},

	/*****************
	 **** Bahrain ****
	 *****************/

	/* Bahrain Telecommunications Company */
	{
		426, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BATELCO",
		"Bahrain Telecommunications Company"
	},

	/* MTC Vodafone (Bahrain) B.S.C. */
	{
		426, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MTC VODAFONE BH",
		"MTC Vodafone (Bahrain) B.S.C."
	},

	/* MTC Vodafone (Bahrain) B.S.C. */
	{
		426, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MTC VODAFONE BH",
		"MTC Vodafone (Bahrain) B.S.C."
	},

	/* MTC Vodafone (Bahrain) B.S.C. */
	{
		426, 2, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "MTC VODAFONE BH",
		"MTC Vodafone (Bahrain) B.S.C."
	},

	/***************
	 **** Qatar ****
	 ***************/

	/* Q-Tel */
	{
		427, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "QAT QATARNET",
		"Q-Tel"
	},

	/* Q-Tel */
	{
		427, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "QAT QATARNET",
		"Q-Tel"
	},

	/******************
	 **** Mongolia ****
	 ******************/

	/* MobiCom */
	{
		428, 99, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MN MobiCom",
		"MobiCom"
	},

	/***************
	 **** Nepal ****
	 ***************/

	/* Nepal Telecommunications Corporation */
	{
		429, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Nepal",
		"Nepal Telecommunications Corporation"
	},

	/**************
	 **** Iran ****
	 **************/

	/* TCI */
	{432, 11, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IR-TCI", "TCI"},

	/* KFZO */
	{432, 14, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IR KISH", "KFZO"},

	/* Mobile Telecommunications Company of Esfahan */
	{
		432, 19, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IR MTCE",
		"Mobile Telecommunications Company of Esfahan"
	},

	/* Rafsanjan Industrial Complex (Coop) */
	{432, 32, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Taliya", "Taliya"},

	/********************
	 **** Uzbekistan ****
	 ********************/

	/* Buztel */
	{434, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Buztel", "Buztel"},

	/* JV Uzmacom */
	{434, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "UZMACOM", "Uzmacom"},

	/* DAEWOO Unitel Company */
	{
		434, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "UZB DAEWOO-GSM",
		"DAEWOO Unitel Company"
	},

	/* DAEWOO Unitel Company */
	{
		434, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "UZB DAEWOO-GSM",
		"DAEWOO Unitel Company"
	},

	/* Coscom */
	{
		434, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "UZB CSOCOM GSM",
		"Coscom"
	},

	/* Uzdunrobita JV */
	{
		434, 7, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "UZB-UZD",
		"Uzdunrobita JV"
	},

	/* Uzdunrobita JV */
	{
		434, 7, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "UZB-UZD",
		"Uzdunrobita JV"
	},

	/********************
	 *** Tajikistan ****
	 ********************/

	/* JSC Somoncom */
	{
		436, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Somoncom",
		"JSC Somoncom"
	},

	/* Indigo Tajikistan */
	{
		436, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Indigo-T",
		"Indigo Tajikistan"
	},

	/* TT Mobile, Closed joint-stock company */
	{
		436, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TJK MLT",
		"TT Mobile, Closed joint-stock company"
	},

	/* TT Mobile, Closed joint-stock company  */
	{
		436, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TJK MLT",
		"TT Mobile, Closed joint-stock company "
	},

	/* JOSA Babilon-Mobile */
	{
		436, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Babilon-M",
		"JOSA Babilon-Mobile"
	},

	/* Tajik Tel */
	{
		436, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TJT - Tajik Tel",
		"Tajik Tel"
	},

	/*************************
	   **** Kyrgyz Republic ****
	 *************************/

	/* Bitel Limited */
	{
		437, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BITEL KGZ",
		"Bitel Limited"
	},

	/* BiMoCom Ltd */
	{437, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MEGACOM", "MEGACOM"},

	/* BiMoCom Ltd */
	{437, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MEGACOM", "MEGACOM"},

	/**********************
	 **** Turkmenistan ****
	 **********************/

	/* Barash Communication Technologies Inc. */
	{
		438, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BCTI",
		"Barash Communication Technologies Inc."
	},

	/**********************
	 **** JAPAN ****
	 **********************/

	/*  NTT DoCoMo, Inc */
	{
		440, 10, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "JP DoCoMo",
		"NTT DoCoMo, Inc"
	},

	/*  J-phone */
	{
		440, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SoftBank",
		"SoftBank"
	},

	/*  J-phone */
	{440, 20, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "SoftBank", "SoftBank"},

	/**********************
	 **** KOREA ****
	 **********************/

	/* Added for testing */

	{
		450, 1, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "SAMSUNG 3G",
		"SAMSUNG 3G"
	},

	/* "KT Freetel Co., Ltd" */
	{450, 2, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "KTF", "KTF"},

	/* SK Telecom */
	{450, 5, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "SKT", "SK Telecom"},

	/* "KT Freetel Co., Ltd" */
	{450, 8, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "KTF", "KTF"},

	/*****************
	 **** Vietnam ****
	 *****************/

	/* Vietnam Mobile Telecom Service */
	{
		452, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "VN MOBIFONE",
		"Vietnam Mobile Telecom Service"
	},

	/* Vietnam Telecoms Services Co (GPC) */
	{
		452, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "VN VINAPHONE",
		"Vietnam Telecoms Services Co (GPC)"
	},

	/* Vietel Corporation */
	{
		452, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "VIETTEL",
		"Viettel Mobile"
	},

	/*******************
	 **** Hong Kong ****
	 *******************/

	/* Hong Kong CSL Limited */
	{
		454, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CSL",
		"Hong Kong CSL Limited"
	},

	/* Hong Kong CSL Limited */
	{
		454, 0, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "CSL",
		"Hong Kong CSL Limited"
	},

	/* Hong Kong CSL Limited */
	{454, 0, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "CSL", "HK CSL"},

	/* Hong Kong CSL Limited */
	{454, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CSL", "HK CSL"},

	/* Hong Kong CSL Limited */
	{454, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "CSL", "HK CSL"},

	/* Hong Kong CSL Limited */
	{
		454, 2, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "CSL",
		"Hong Kong CSL Limited"
	},

	/* Hutchison Telecom (HK) Ltd */
	{
		454, 3, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "3",
		"Hutchison Telecom (HK) Ltd"
	},

	/* Hutchison Telecom (HK) Ltd */
	{
		454, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "3 (2G)",
		"Hutchison Telecom (HK) Ltd"
	},

	/* Hutchison Telecom (HK) Ltd */
	{
		454, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "3 (2G)",
		"Hutchison Telecom (HK) Ltd"
	},

	/* SmarTone Mobile Comms Ltd */
	{
		454, 6, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SmarToneVodafone",
		"SmarTone Mobile Comms Ltd"
	},

	/* SmarTone Mobile Comms Ltd */
	{
		454, 6, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SmarToneVodafone",
		"SmarTone Mobile Comms Ltd"
	},

	/* New World PCS Ltd */
	{
		454, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "NEW WORLD",
		"New World PCS Ltd"
	},

	/* Peoples Telephone Co Ltd */
	{
		454, 12, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "PEOPLES",
		"Peoples Telephone Co Ltd"
	},

	/* SmarTone Mobile Comms Ltd */
	{
		454, 15, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "454-15",
		"SmarTone Mobile Comms Ltd"
	},

	/* SUNDAY Communications Ltd */
	{
		454, 16, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SUNDAY",
		"SUNDAY Communications Ltd"
	},

	/* Hong Kong CSL Limited */
	{
		454, 18, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CSL",
		"Hong Kong CSL Limited"
	},

	/* Hong Kong CSL Limited */
	{
		454, 18, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "CSL",
		"Hong Kong CSL Limited"
	},

	/* Hong Kong CSL Limited */
	{454, 18, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "CSL", "HK CSL"},

	/* PCCW Mobile */
	{454, 19, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "PCCW", "PCCW Mobile"},

	/* P Plus Communicatoins Ltd */
	{
		454, 22, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "HK P PLUS",
		"P Plus Communicatoins Ltd"
	},

	/***************
	 **** Macau ****
	 ***************/

	/* SmarTone Mobile Communications (Macau) Ltd */
	{
		455, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SmarTone",
		"SmarTone Mobile Communications (Macau) Ltd"
	},

	/* SmarTone Mobile Communications (Macau) Ltd */
	{
		455, 0, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SmarTone",
		"SmarTone Mobile Communications (Macau) Ltd"
	},

	/* C.T.M. */
	{455, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CTM", "C.T.M."},

	/* C.T.M. */
	{455, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "CTM", "C.T.M."},

	/* Hutchison Telephone (Macau) Company Limited */
	{
		455, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "HT Macau",
		"Hutchison Telephone (Macau) Company Limited"
	},

	/* Hutchison Telephone (Macau) Company Limited */
	{
		455, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "HT Macau",
		"Hutchison Telephone (Macau) Company Limited"
	},

	/******************
	 **** Cambodia ****
	 ******************/

	/* CamGSM */
	{
		456, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MOBITEL - KHM",
		"CamGSM"
	},

	/* Cambodia Samart Communication Co Ltd */
	{
		456, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "KHM-Hello GSM",
		"Cambodia Samart Communication Co Ltd"
	},

	/* Cambodia Shinawatra */
	{
		456, 18, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800,
		"CAMBODIA SHINAWATRA ", "Cambodia Shinawatra Co. Ltd"
	},

	/*************
	 **** Lao ****
	 *************/

	/* Lao Telecommunications */
	{
		457, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LAO GSM",
		"Lao Telecommunications"
	},

	/* Enterprise of Telecommunications Lao (ETL) */
	{
		457, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ETL MOBILE NETWORK",
		"Enterprise of Telecommunications Lao (ETL)"
	},

	/* Lao Asia Telecommunication State Enterprise (LAT) */
	{
		457, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "45703",
		"Lao Asia Telecommunication State Enterprise (LAT)"
	},

	/* Lao Asia Telecommunication State Enterprise (LAT) */
	{
		457, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "45703",
		"Lao Asia Telecommunication State Enterprise (LAT)"
	},

	/* Millicom Lao Co Ltd */
	{
		457, 8, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TANGO LAO",
		"Millicom Lao Co Ltd"
	},

	/* Millicom Lao Co Ltd */
	{
		457, 8, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TANGO LAO",
		"Millicom Lao Co Ltd"
	},

	/***************
	 **** China ****
	 ***************/

	/* China Mobile */
	{
		460, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CMCC",
		"CHINA MOBILE"
	},

	/* China Unicom */
	{
		460, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CU-GSM",
		"CHINA UNICOM GSM"
	},

	/****************
	 **** Taiwan ****
	 ****************/

	/* Far EasTone Telecommunications GSM 900/1800 */
	{
		466, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "FarEasTone",
		"Far EasTone Telecommunications GSM 900/1800"
	},

	/* Far EasTone Telecommunications GSM 900/1800 */
	{
		466, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "FarEasTone",
		"Far EasTone Telecommunications GSM 900/1800"
	},

	/* KG Telecom */
	{
		466, 6, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TWN Tuntex GSM 1800",
		"KG Telecom"
	},

	/*ACeS International Limited (AIL) */
	{
		466, 68, SETTING_PREFERRED_NETWORK_TYPE_GSM_SAT, "ACeS",
		"ACeS International Limited (AIL)"
	},

	/* KG Telecom */
	{
		466, 88, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "KGT-ONLINE",
		"KG Telecom"
	},

	/* Taiwan 3G Mobile Network */
	{
		466, 89, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "ViBo",
		"Taiwan 3G Mobile Network"
	},

	/* Chunghwa Telecom */
	{
		466, 92, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Chunghwa",
		"Chunghwa Telecom"
	},

	/* Chunghwa Telecom */
	{
		466, 92, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Chunghwa",
		"Chunghwa Telecom"
	},

	/* MobiTai */
	{466, 93, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MoBiTai", "MobiTai"},

	/* Taiwan Cellular Corporation */
	{
		466, 97, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Taiwan Mobile",
		"Taiwan Cellular Corporation"
	},

	/* TransAsia Telecommunications */
	{
		466, 99, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TransAsia Telecom",
		"TransAsia Telecommunications"
	},

	/********************
	 ****			****
	 ********************/

	/* Korea Posts and Telecommunications Corporation (KPTC) */
	{467, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SUNNET", "SUN NET"},

	/* NEAT&T Ltd. */
	{
		467, 193, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "KP SUN",
		"NEAT&T Ltd."
	},

	/********************
	 **** Bangladesh ****
	 ********************/

	/* Grameen Phone Ltd */
	{
		470, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BGD-GP",
		"Grameen Phone Ltd"
	},

	/* TM International (Bangladesh) Ltd */
	{
		470, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BGD AKTEL",
		"TM International (Bangladesh) Ltd"
	},

	/* Sheba Telecom (Pvt.) Ltd. */
	{
		470, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Banglalink",
		"Banglalink"
	},

	/* Teletalk Bangladesh Ltd */
	{470, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "bMobile", "Teletalk"},

	/* Sheba Telecom (PVT) Ltd */
	{
		470, 19, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BD ShebaWorld",
		"Sheba Telecom (PVT) Ltd"
	},

	/******************
	 **** Maldives ****
	 ******************/

	/* Dhivehi Raajjeyge Gulhun Private Ltd GSM 900 */
	{
		472, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MV DHIMOBILE",
		"Dhivehi Raajjeyge Gulhun Private Ltd GSM 900"
	},

	/* Wataniya Telecom Maldives Pvt. Ltd */
	{472, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "WMOBILE", "WMOBILE"},

	/******************
	 **** Malaysia ****
	 ******************/

	/* Maxis Communications Berhad */
	{
		502, 12, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MY MAXIS",
		"Maxis Communications Berhad"
	},

	/* Maxis Communications Berhad */
	{
		502, 12, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MY MAXIS",
		"Maxis Communications Berhad"
	},

	/* Maxis Mobile Sdn Bhd */
	{
		502, 12, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "MY MAXIS",
		"Maxis Mobile Sdn Bhd"
	},

	/* Telekom Cellular Sdn Bhd */
	{
		502, 13, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MY TMTOUCH",
		"Telekom Cellular Sdn Bhd"
	},

	/* Celcom (Malaysia) Sdn Bhd */
	{
		502, 13, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "CELCOM",
		"Celcom Malaysia"
	},

	/* DiGi Telecommunications Sdn Bhd */
	{
		502, 16, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "DiGi",
		"DiGi Telecommunications Sdn Bhd"
	},

	/* TIMECel Sdn Bhd */
	{
		502, 17, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MY ADAM",
		"TIMECel Sdn Bhd"
	},

	/* Celcom (Malaysia) Sdn Bhd */
	{
		502, 19, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MY CELCOM",
		"Celcom (Malaysia) Sdn Bhd"
	},

	/*******************
	 **** Australia ****
	 *******************/

	/* Telstra Mobile Comms */
	{
		505, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Telstra",
		"Telstra Mobile Comms"
	},

	/* Telstra Mobile Comms */
	{
		505, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Telstra",
		"Telstra Mobile Comms"
	},

	/* Singtel Optus Limited */
	{
		505, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "YES OPTUS",
		"Singtel Optus Limited"
	},

	/* Vodafone Pacific Limited */
	{
		505, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone AU",
		"Vodafone Pacific Limited"
	},

	/* Vodafone Pacific Limited */
	{
		505, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "vodafone AU",
		"Vodafone Pacific Limited"
	},

	/* Hutchison 3G Australia Pty Limited */
	{
		505, 6, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "3Telstra",
		"Hutchison/Telstra 3G Australia Pty Limited"
	},

	/* One.Tel Networks */
	{
		505, 8, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "One.Tel",
		"One.Tel Networks"
	},

	/*******************
	 **** Satellite ****
	 *******************/

	/* ACeS International Limited (AIL) - INDONESIA */
	{
		510, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_SAT, "ACeS",
		"ACeS International Limited (AIL) - INDONESIA"
	},

	/*******************
	 **** Indonesia ****
	 *******************/

	/* PT Indonesian Satellite Corporation Tbk (INDOSAT) */
	{
		510, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IND INDOSAT",
		"PT Indonesian Satellite Corporation Tbk (INDOSAT)"
	},

	/* PT Indonesian Satellite Corporation Tbk (INDOSAT) */
	{
		510, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IND INDOSAT",
		"PT Indonesian Satellite Corporation Tbk (INDOSAT)"
	},

	/* PT Natrindo Telepon Seluler */
	{
		510, 8, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "LIPPO TEL",
		"PT Natrindo Telepon Seluler"
	},

	/* PT Telekomunikasi Selular */
	{
		510, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IND TELKOMSEL",
		"PT Telekomunikasi Selular"
	},

	/* PT Telekomunikasi Selular */
	{
		510, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IND TELKOMSEL",
		"PT Telekomunikasi Selular"
	},

	/* Excelcom */
	{
		510, 11, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IND - Excelcom",
		"Excelcom"
	},

	/* Excelcom */
	{
		510, 11, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IND - Excelcom",
		"Excelcom"
	},

	/* PT Indonesian Satellite Corporation Tbk (INDOSAT) */
	{
		510, 21, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IND IM3",
		"PT Indonesian Satellite Corporation Tbk (INDOSAT)"
	},

	/* PT Indonesian Satellite Corporation Tbk (INDOSAT) */
	{
		510, 21, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IND INDOSAT",
		"PT Indonesian Satellite Corporation Tbk (INDOSAT)"
	},

	/* PT Indonesian Satellite Corporation Tbk (INDOSAT) */
	{
		510, 21, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "IND INDOSAT",
		"PT Indonesian Satellite Corporation Tbk (INDOSAT)"
	},

	/* PT Hutchison CP Telecommunications */
	{510, 89, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "3", "3"},

	/* PT Hutchison CP Telecommunications */
	{510, 89, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "3", "3"},

	/********************
	 **** TLS ****
	 ********************/

	/* Timor Telecom. */
	{
		514, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TLS-TT",
		"Timor Telecom"
	},

	/********************
	 **** Philipines ****
	 ********************/

	/* Isla Comms Co. Inc. */
	{
		515, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ISLACOM",
		"Isla Comms Co. Inc."
	},

	/* Globe Telecom, GMCR Inc. */
	{
		515, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Globe Telecom-PH",
		"Globe Telecom, GMCR Inc."
	},

	/* Globe Telecom, GMCR Inc. */
	{
		515, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Globe Telecom-PH",
		"Globe Telecom, GMCR Inc."
	},

	/* Smart Communications Inc. */
	{
		515, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SMART",
		"Smart Communications Inc."
	},

	/* Smart Communications Inc. */
	{
		515, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SMART",
		"Smart Communications Inc."
	},

	/* Digital Telecommunications Phils, Inc */
	{
		515, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "PH Sun Cellular",
		"Digital Telecommunications Phils, Inc"
	},

	/*******************
	 **** Satellite ****
	 *******************/

	/* ACeS International Limited (AIL) - PHILIPPINES */
	{
		515, 11, SETTING_PREFERRED_NETWORK_TYPE_GSM_SAT, "ACeS",
		"ACeS International Limited (AIL) - PHILIPPINES"
	},

	/******************
	 **** Thailand ****
	 ******************/

	/* Advanced Info Service Plc */
	{
		520, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TH GSM",
		"Advanced Info Service Plc"
	},

	/* Wireless Comm. Services Co. */
	{
		520, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TH WCS",
		"Wireless Comm. Services Co."
	},

	/* ACT Mobile Company, Limited */
	{
		520, 15, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "TH ACT 1900",
		"ACT Mobile Company, Limited"
	},

	/* Total Access Comms Co. */
	{
		520, 18, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TH-DTAC",
		"Total Access Comms Co."
	},

	/* ACeS International Limited (AIL) - THAILAND */
	{
		520, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_SAT, "ACeS",
		"ACeS International Limited (AIL) - THAILAND"
	},

	/* Digital Phone Co Ltd */
	{
		520, 23, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TH GSM 1800",
		"Digital Phone Co Ltd"
	},

	/* TA Orange Company Ltd */
	{
		520, 99, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Orange Th",
		"TA Orange Company Ltd"
	},

	/*******************
	 **** Singapore ****
	 *******************/

	/* Singapore Telecom Mobile Pte Ltd */
	{
		525, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SingTel",
		"Singapore Telecom Mobile Pte Ltd"
	},

	/* Singapore Telecom Mobile Pte Ltd */
	{
		525, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SingTel",
		"Singapore Telecom Mobile Pte Ltd"
	},

	/* Singapore Telecom GSM 1800 */
	{
		525, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SingTel-G18",
		"Singapore Telecom Mobile Pte Ltd"
	},

	/* MobileOne (Asia) Pte Ltd */
	{
		525, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SGP-M1-3GSM",
		"MobileOne (Asia) Pte Ltd"
	},

	/* MobileOne (Asia) Pte Ltd */
	{
		525, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SGP-M1-3GSM",
		"MobileOne (Asia) Pte Ltd"
	},

	/* MobileOne (Asia) Pte Ltd */
	{
		525, 3, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "SGP-M1-3GSM",
		"MobileOne (Asia) Pte Ltd"
	},

	/* StarHub Mobile Pte Ltd */
	{
		525, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "STARHUB",
		"StarHub Mobile Pte Ltd"
	},

	/* StarHub Mobile Pte Ltd */
	{
		525, 5, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "STARHUB",
		"StarHub Mobile Pte Ltd"
	},

	/****************
	 **** Brunei ****
	 ****************/

	/* B-Mobile Communications Sdn Bhd */
	{528, 2, SETTING_PREFERRED_NETWORK_TYPE_UMTS, "b-mobile", "b-mobile"},

	/* DataStream Technology */
	{
		528, 11, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BRU-DSTCom",
		"DataStream Technology"
	},

	/*********************
	 **** New Zealand ****
	 *********************/

	/* Vodafone Mobile NZ Limited */
	{
		530, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone NZ",
		"Vodafone Mobile NZ Limited"
	},

	/*********************
	 **** PNG ****
	 *********************/

	/* Pacific Mobile Communications */
	{
		537, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "PNGBMobile",
		"Pacific Mobile Communications"
	},

	/***************
	 **** Tonga ****
	 ***************/

	/* Tonga Communications Corporation */
	{
		539, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "U-CALL",
		"Tonga Communications Corporation"
	},

	/***************
	 ****	   ****
	 ***************/

	/* Solomon Telekom Co Ltd */
	{540, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BREEZE", "BREEZE"},

	/*******************
	 **** Satellite ****
	 *******************/

	/* ACeS International Limited (AIL) - REGIONAL */
	{
		541, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_SAT, "ACeS",
		"ACeS International Limited (AIL) - REGIONAL"
	},

	/*****************
	 **** Vanuatu ****
	 *****************/

	/* Telecom Vanuatu Ltd */
	{
		541, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "VUT SMILE",
		"Telecom Vanuatu Ltd"
	},

	/**************
	 **** Fiji ****
	 **************/

	/* Vodafone Fiji Limited */
	{
		542, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "FJ VODAFONE",
		"Vodafone Fiji Limited"
	},

	/************************
	 **** American Samoa ****
	 ************************/

	/* Blue Sky */
	{
		544, 11, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Blue Sky",
		"Blue Sky"
	},

	/**************
	 **** KI ****
	 **************/

	/* Telecom Services Kiribati Limited (TSKL) */
	{
		545, 9, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "KL-Frigate",
		"Telecom Services Kiribati Limited (TSKL)"
	},

	/***********************
	 **** New Caledonia ****
	 ***********************/

	/* OPT New Caledonia */
	{
		546, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "NCL MOBILIS",
		"OPT New Caledonia"
	},

	/**************************
	 **** French Polynesia ****
	 **************************/

	/* Tikiphone S.A */
	{
		547, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "F-VINI",
		"Tikiphone S.A"
	},

	/********************
	 **** Cook Islands ****
	 ********************/

	/* Telecom Cook Islands */
	{
		548, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "KOKANET",
		"Telecom Cook Islands"
	},

	/********************
	 **** Micronesia ****
	 ********************/

	/* FSM Telecommunications Corporation */
	{
		550, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "FSM Telecom",
		"FSM Telecommunications Corporation"
	},

	/***************
	 **** Palau ****
	 ***************/

	/* Palau Mobile Corporation */
	{
		552, 80, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "PLWPMC",
		"Palau Mobile"
	},

	/***************
	 **** Egypt ****
	 ***************/

	/* The Egyptian Company for Mobile Services */
	{
		602, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "EGY MobiNiL",
		"The Egyptian Company for Mobile Services"
	},

	/* Vodafone Egypt Telecommunications S.A.E */
	{
		602, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "vodafone EG",
		"Vodafone Egypt Telecommunications S.A.E"
	},

	/*****************
	 **** Algeria ****
	 *****************/

	/* Algerie Telecom */
	{
		603, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900,
		"ALGERIAN MOBILE NETWORK", "Algerie Telecom"
	},

	/* Orascom Telecom Algerie Spa */
	{
		603, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Djezzy",
		"Orascom Telecom Algerie Spa"
	},

	/* Orascom Telecom Algerie Spa */
	{
		603, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Djezzy",
		"Orascom Telecom Algerie Spa"
	},

	/* Wataniya Telecom Algerie */
	{603, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "NEDJMA", "Nedjma"},

	/* Wataniya Telecom Algerie */
	{603, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "NEDJMA", "Nedjma"},

	/*****************
	 **** Morocco ****
	 *****************/

	/* Medi Telecom */
	{
		604, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MOR MEDITEL",
		"Medi Telecom"
	},

	/* Itissalat Al-Maghrib S.A */
	{604, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MOR IAM", "IAM"},

	/*****************
	 **** Tunisia ****
	 *****************/

	/* Tunisie Telecom */
	{
		605, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TUNISIE TELECOM",
		"Tunisie Telecom"
	},

	/* Orascom Telecom Tunisie */
	{
		605, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TUNISIANA",
		"Orascom Telecom Tunisie"
	},

	/* Libyana Mobile Phone  */
	{
		606, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LIBYANA",
		"Libyana Mobile Phone "
	},

	/* AL MADAR Telecomm Company */
	{606, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "606 01", "606 - 01"},

	/****************
	 **** Gambia ****
	 ****************/

	/* Gambia Telecommunications Cellular Company Ltd */
	{
		607, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GAMCEL",
		"Gambia Telecommunications Cellular Company Ltd"
	},

	/* Africell (Gambia) Ltd */
	{
		607, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AFRICELL",
		"Africell (Gambia) Ltd"
	},

	/*****************
	 **** Senegal ****
	 *****************/

	/* Sonatel */
	{608, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SN ALIZE", "Sonatel"},

	/* Sentel GSM */
	{
		608, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SN-SENTEL SG",
		"Sentel GSM"
	},

	/*****************
	 **** Maurital ****
	 *****************/

	/* MATTEL */
	{609, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MR MATTEL", "MATTEL"},

	/* MAURITEL MOBILES */
	{
		609, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MAURITEL",
		"MAURITEL"
	},

	/**************
	 **** Mali ****
	 **************/

	/* Malitel-SA */
	{
		610, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MALITEL ML",
		"Malitel-SA"
	},

	/* IKATEL SA */
	{
		610, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "IKATEL ML",
		"IKATEL SA"
	},

	/****************
	 **** Guinea ****
	 ****************/

	/* Spacetel Guinee SA */
	{
		611, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Mobilis",
		"Spacetel Guinee SA"
	},

	/* Sotelgui SA */
	{
		611, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GN LAGUI",
		"Sotelgui SA"
	},

	/* Areeba Guinea */
	{611, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Areeba", "Areeba"},

	/* Areeba Guinea */
	{611, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Areeba", "Areeba"},

	/***********************
	 **** Cote d'Ivoire ****
	 ***********************/

	/* CORA de COMSTAR */
	{
		612, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CI CORA",
		"CORA de COMSTAR"
	},

	/* Atlantique Cellulaire */
	{612, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ACELL-CI", "A-Cell"},

	/* Atlantique Cellulaire */
	{612, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "ACELL-CI", "A-Cell"},

	/* S.I.M. */
	{
		612, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Orange CI",
		"Orange CI"
	},

	/* Loteny Telecom (SA) */
	{
		612, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TELECEL-CI",
		"Loteny Telecom (SA)"
	},

	/**********************
	 **** Burkina Faso ****
	 **********************/

	/* Onatel */
	{613, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Onatel", "Onatel"},

	/* Celtel Burkina Faso */
	{
		613, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BF CELTEL",
		"Celtel Burkina Faso"
	},

	/***************
	 **** Niger ****
	 ***************/

	/* Sahel-Com */
	{
		614, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SAHELCOM",
		"SahelCom"
	},

	/* Celtel Niger */
	{
		614, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "NE CELTEL",
		"Celtel Niger"
	},

	/* Telecel Niger SA */
	{
		614, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "NE TELECEL",
		"Telecel Niger SA"
	},

	/**************
	 **** Togo ****
	 **************/

	/* Togo Cellulaire */
	{
		615, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TG-TOGO CELL",
		"Togo Telecom"
	},

	/***************
	 **** Benin ****
	 ***************/

	/* LIBERCOM */
	{
		616, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LIBERCOM",
		"LIBERCOM"
	},

	/* Telecel Benin Ltd */
	{
		616, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TELECEL BENIN",
		"Telecel Benin Ltd"
	},

	/* Spacetel-Benin */
	{
		616, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BJ BENINCELL",
		"Spacetel-Benin"
	},

	/* Bell Benin Communications (BBCOM) */
	{
		616, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900,
		"BELL BENIN COMMUNICATION", "Bell Benin Communications (BBCOM)"
	},

	/* Bell Benin Communications (BBCOM) */
	{
		616, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800,
		"BELL BENIN COMMUNICATION", "Bell Benin Communications (BBCOM)"
	},

	/*******************
	 **** Mauritius ****
	 *******************/

	/* Cellplus Mobile Comms */
	{
		617, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CELLPLUS-MRU",
		"Cellplus Mobile Comms"
	},

	/* Emtel Ltd */
	{
		617, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "EMTEL-MRU",
		"Emtel Ltd"
	},

	/*****************
	 **** Liberia ****
	 *****************/

	/* Lonestar Communications Corporation */
	{
		618, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LBR Lonestar Cell",
		"Lonestar Communications Corporation"
	},

	/* Atlantic Wireless (Liberia) Inc.  */
	{
		618, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LIBERCEL",
		"LIBERCELL"
	},

	/* "Celcom Telecommunications, Inc" */
	{
		618, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Celcom",
		"Celcom Telecommunications"
	},

	/* "Celcom Telecommunications, Inc" */
	{
		618, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Celcom",
		"Celcom Telecommunications"
	},

	/****************
	 **** SL ****
	 ****************/

	/* Celtel (SL) Limited */
	{
		619, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CELTEL SL",
		"Celtel (SL) Limited"
	},

	/* Celtel (SL) Limited */
	{
		619, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MILLICOM SL",
		"Millicom SL"
	},

	/***************
	 **** Ghana ****
	 ***************/

	/* ScanCom Ltd */
	{
		620, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GH SPACEFON",
		"ScanCom Ltd"
	},

	/* Ghana Telecommunications Company Ltd */
	{
		620, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GH ONEtouch",
		"Ghana Telecommunications Company Ltd"
	},

	/* Millicom Ghana Limited */
	{
		620, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GH-MOBITEL",
		"Millicom Ghana Limited"
	},

	/*****************
	 **** Nigeria ****
	 *****************/

	/* Econet Wireless Nigeria Ltd */
	{
		621, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ECONET NG",
		"Econet Wireless Nigeria Ltd"
	},

	/* Econet Wireless Nigeria Ltd */
	{
		621, 20, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "ECONET NG",
		"Econet Wireless Nigeria Ltd"
	},

	/* MTN Nigeria Communications Limited */
	{
		621, 30, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MTN - NG",
		"MTN Nigeria Communications Limited"
	},

	/* MTN Nigeria Communications Limited */
	{
		621, 30, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MTN - NG",
		"MTN Nigeria Communications Limited"
	},

	/* Nigerian Telecommunications Limited (NITEL) */
	{
		621, 40, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "NG Mtel",
		"Nigerian Telecommunications Limited (NITEL)"
	},

	/* Nigerian Telecommunications Limited (NITEL) */
	{
		621, 40, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "NG Mtel",
		"Nigerian Telecommunications Limited (NITEL)"
	},

	/* Globacom Ltd. */
	{
		621, 50, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Glo NG",
		"Globacom Ltd."
	},

	/* Globacom Ltd. */
	{
		621, 50, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Glo NG",
		"Globacom Ltd."
	},

	/**************
	 **** Chad ****
	 **************/

	/* CelTel Tchad SA */
	{
		622, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CELTEL TCD",
		"CelTel Tchad SA"
	},

	/* Tchad Mobile SA */
	{
		622, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TD LIBERTIS",
		"Tchad Mobile SA"
	},

	/******************
	 **** Cameroon ****
	 ******************/

	/* MTN Cameroon Ltd */
	{
		624, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MTN CAM",
		"MTN Cameroon Ltd"
	},

	/* Orange Cameroun S.A. */
	{
		624, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Orange CAM",
		"Orange Cameroun S.A."
	},

	/********************
	 **** Cabo Verde ****
	 ********************/

	/* Cabo Verde Telecom */
	{
		625, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CPV MOVEL",
		"Cabo Verde Telecom"
	},

	/********************
	 **** STP  ****
	 ********************/

	/* Companhia Santomense de Telecomunicacoes SARL */
	{
		626, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "STP CSTmovel",
		"Companhia Santomense de Telecomunicacoes SARL"
	},

	/********************
	 **** GNQ  ****
	 ********************/

	/* GETESA */
	{627, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GNQ01", "GETESA"},

	/***************
	 **** Gabon ****
	 ***************/

	/* Libertis S.A. */
	{
		628, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LIBERTIS",
		"Libertis S.A."
	},

	/* Telecel Gabon SA */
	{
		628, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GAB TELECEL",
		"Telecel Gabon SA"
	},

	/* Celtel Gabon SA */
	{
		628, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CELTEL GA",
		"Celtel Gabon SA"
	},

	/***************
	 **** Congo ****
	 ***************/

	/* CelTel Congo SA */
	{
		629, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CELTEL CD",
		"CelTel Congo SA"
	},

	/* Libertis Telecom */
	{
		629, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "COG LIBERTIS",
		"Libertis Telecom"
	},

	/* Vodacom Congo (RDC) sprl */
	{
		630, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "VODACOM CD",
		"Vodacom Congo (RDC) sprl"
	},

	/* Vodacom Congo (RDC) sprl */
	{
		630, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "VODACOM CD",
		"Vodacom Congo (RDC) sprl"
	},

	/* CelTel Congo SA */
	{
		630, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CELTEL RC",
		"CelTel Congo SA"
	},

	/* CELLCO Sarl */
	{
		630, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CELLCO GSM",
		"CELLCO Sarl"
	},

	/* Supercell Sprl */
	{
		630, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SCELL",
		"Supercell Sprl"
	},

	/* Supercell Sprl */
	{
		630, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SCELL",
		"Supercell Sprl"
	},

	/* SAIT Telecom SPRL */
	{
		630, 89, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "CD OASIS",
		"SAIT Telecom SPRL"
	},

	/*****************
	 **** Somalia ****
	 *****************/

	/* UNITEL S.a.r.l. */
	{
		631, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "UNITEL",
		"UNITEL S.a.r.l."
	},

	/* Telsom Mobile Somalia */
	{
		631, 82, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "telsom",
		"Telsom Mobile Somalia"
	},

	/********************
	 **** Seychelles ****
	 ********************/

	/* Cable & Wireless (Seychelles) Ltd */
	{
		633, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SEYCEL",
		"Cable & Wireless (Seychelles) Ltd"
	},

	/* UNITEL S.a.r.l. */
	{631, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "UNITEL", "UNITEL"},

	/* Telecom (Seychelles Limited) */
	{
		633, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SEZ AIRTEL",
		"Telecom (Seychelles Limited)"
	},

	/* Guinetel */
	{632, 7, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "GTM", "Guinetel"},

	/* Cable & Wireless (Seychelles) */
	{
		633, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "C&W SEY",
		"CABLE & WIRELESS"
	},

	/* Telecom (Seychelles) Ltd */
	{633, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AIRTEL", "AIRTEL"},

	/***************
	 **** Sudan ****
	 ***************/

	/* Mobitel */
	{
		634, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MobiTel SDN",
		"Mobitel"
	},

	/* Bashair Telecom Co.Ltd */
	{634, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "areeba", "Areeba"},

	/* Bashair Telecom Co.Ltd */
	{634, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "areeba", "Areeba"},

	/****************
	 **** Rwanda ****
	 ****************/

	/* Rwandacell SARL */
	{
		635, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "R-CELL",
		"Rwandacell SARL"
	},

	/******************
	 **** Ethiopia ****
	 ******************/

	/* Ethiopian Telecoms Auth. */
	{
		636, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ETH-MTN",
		"Ethiopian Telecoms Auth."
	},

	/******************
	 **** Somalia ****
	 ******************/

	/* Telesom Company */
	{
		637, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SOMTELESOM",
		"Telesom Company"
	},

	/* Somafone FZLLC  */
	{
		637, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SOMAFONE",
		"SOMAFONE"
	},

	/* Somafone FZLLC  */
	{
		637, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SOMAFONE",
		"SOMAFONE"
	},

	/* Golis Telecommunications Company Ltd */
	{637, 30, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Golis", "Golis"},

	/* Telsom Mobile Somalia*/
	{
		637, 82, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Telsom Mobile",
		"Telsom Mobile Somalia"
	},

	/******************
	 **** DJ ****
	 ******************/

	/* Djibouti Telecom SA */
	{
		638, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "DJ EVATIS",
		"Djibouti Telecom SA"
	},

	/***************
	 **** Kenya ****
	 ***************/

	/* Safaricom Limited */
	{
		639, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Safaricom",
		"Safaricom Limited"
	},

	/* Kencell Communications Ltd */
	{
		639, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "YES!",
		"Kencell Communications Ltd"
	},

	/******************
	 **** Tanzania ****
	 ******************/

	/* Tritel (T) Ltd */
	{
		640, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TRITEL-TZ",
		"Tritel (T) Ltd"
	},

	/* MIC Tanzania Ltd */
	{
		640, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MOBITEL - TZ",
		"MIC Tanzania Ltd"
	},

	/* MIC Tanzania Limited */
	{640, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MOBITEL", "MOBITEL"},

	/* Zanzibar Telecom */
	{
		640, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ZANTEL-TZ",
		"Zanzibar Telecom"
	},

	/* Zanzibar Telecom */
	{
		640, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "ZANTEL-TZ",
		"Zanzibar Telecom"
	},

	/* Vodacom Tanzania Ltd */
	{
		640, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "VodaCom",
		"Vodacom Tanzania Ltd"
	},

	/* Vodacom Tanzania Ltd */
	{
		640, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "VodaCom",
		"Vodacom Tanzania Ltd"
	},

	/* Celtel Tanzania Limited */
	{
		640, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CELTEL TZ",
		"Celtel Tanzania Limited"
	},

	/* Celtel Tanzania Limited */
	{
		640, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "CELTEL TZ",
		"Celtel Tanzania Limited"
	},

	/* Celtel Tanzania Limited ******
	{ 640, 5, TAPI_NETW_TYPE_GSM_400, "celtel", "celtel" }, *****/

	/****************
	 **** Uganda ****
	 ****************/

	/* Clovergem Celtel */
	{
		641, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "UG CelTel",
		"Clovergem Celtel"
	},

	/* MTN Uganda Ltd */
	{
		641, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MTN-UGANDA",
		"MTN Uganda Ltd"
	},

	/* MTN Uganda Ltd */
	{
		641, 10, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MTN-UGANDA",
		"MTN Uganda Ltd"
	},

	/* Uganda Telecom Ltd */
	{
		641, 11, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "mango",
		"Uganda Telecom Ltd"
	},

	/*****************
	 **** Burundi ****
	 *****************/

	/* Spacetel - Burundi */
	{
		642, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Spacetel BI",
		"Spacetel - Burundi"
	},

	/* Africell PLC Company */
	{
		642, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BUSAFA",
		"Africell PLC Company"
	},

	/* ONATEL  */
	{642, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ONATEL", "ONATEL "},

	/* Telecel-Burundi Company */
	{
		642, 82, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BDITL",
		"Telecel-Burundi Company"
	},

	/********************
	 **** Mozambique ****
	 ********************/

	/* Mocambique Celular Ltd (mCel) */
	{
		643, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MOZ - mCel",
		"Mocambique Celular Ltd (mCel)"
	},

	/* Mocambique Celular Ltd (mCel) */
	{
		643, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MOZ - mCel",
		"Mocambique Celular Ltd (mCel)"
	},

	/* VM, S.A.R.L. */
	{
		643, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "VodaCom-MZ",
		"VM, S.A.R.L."
	},

	/* VM, S.A.R.L. */
	{
		643, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "VodaCom-MZ",
		"VM, S.A.R.L."
	},

	/****************
	 **** Zambia ****
	 ****************/

	/* Celtel Zambia */
	{
		645, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ZM CELTEL",
		"Celtel Zambia"
	},

	/* Telecel Zambia Limited */
	{
		645, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Telecel",
		"Telecel Zambia Limited"
	},

	/********************
	 **** Madagascar ****
	 ********************/

	/* Madacom SA */
	{
		646, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MG Madacom",
		"Madacom SA"
	},

	/* Orange Madagascar S.A. */
	{
		646, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MG ANTARIS",
		"Orange Madagascar S.A."
	},

	/**********************
	 **** Reunion (La) ****
	 **********************/

	/* Orange Reunion */
	{
		647, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Orange re",
		"Orange Reunion"
	},

	/* Orange Reunion */
	{
		647, 0, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Orange re",
		"Orange Reunion"
	},

	/* Outremer Telecom */
	{
		647, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "F-OMT",
		"Outremer Telecom"
	},

	/* Societe Reunionnaise */
	{
		647, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SFR REUNION",
		"Societe Reunionnaise"
	},

	/******************
	 **** Zimbabwe ****
	 ******************/

	/* Net*One Cellular (Pvt) Ltd */
	{
		648, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ZW NET*ONE",
		"Net*One Cellular (Pvt) Ltd"
	},

	/* Telecel Zimbabwe (PVT) Ltd */
	{
		648, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TELECEL ZW",
		"Telecel Zimbabwe (PVT) Ltd"
	},

	/* Econet Wireless (Private) Limited */
	{
		648, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ZW ECONET",
		"Econet Wireless (Private) Limited"
	},

	/*****************
	 **** Namibia ****
	 *****************/

	/* MTC */
	{649, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MTC NAMIBIA", "MTC"},

	/****************
	 **** Malawi ****
	 ****************/

	/* Telekom Network Ltd */
	{
		650, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "MW CP 900",
		"Telekom Network Ltd"
	},

	/* CelTel Limited */
	{
		650, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CELTEL MW",
		"CelTel Limited"
	},

	/*****************
	 **** Lesotho ****
	 *****************/

	/* Vodacom Lesotho (Pty) Ltd */
	{
		651, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "VCL COMMS",
		"Vodacom Lesotho (Pty) Ltd"
	},

	/* Econet Ezi Cel Lesotho (Pty) */
	{
		651, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "LS-ECONET-EZI-CEL",
		"Econet Ezi Cel Lesotho (Pty)"
	},

	/******************
	 **** Botswana ****
	 ******************/

	/* Mascom Wireless (Pty) Limited */
	{
		652, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "BW MASCOM",
		"Mascom Wireless (Pty) Limited"
	},

	/* Orange (Botswana) Pty Limited */
	{
		652, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Orange",
		"Orange (Botswana) Pty Limited"
	},

	/*******************
	 **** Swaziland ****
	 *******************/

	/* Swazi MTN Limited */
	{
		653, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Swazi-MTN",
		"Swazi MTN Limited"
	},

	/* Societe Nationale des Telecommunications (Comores Telecom) */
	{654, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "HURI", "HURI"},

	/**********************
	   **** South Africa ****
	 **********************/

	/* request from vodacom (south africa) 7 Jan 2005 */

	/* Vodacom (Pty) Ltd */
	{
		655, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RSA Vodacom",
		"Vodacom (Pty) Ltd"
	},

	/* Cell C (Pty) Ltd */
	{
		655, 7, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "RSA Cell C",
		"Cell C (Pty) Ltd"
	},

	/* MTN - Mobile Telephone Networks (Pty) Ltd. */
	{
		655, 10, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "RSA MTN",
		"MTN - Mobile Telephone Networks (Pty) Ltd."
	},

	/*********************
	 ****			****
	 *********************/

	/* Belize Telecommunications Ltd */
	{
		702, 67, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "BTL",
		"Belize Telecommunications Ltd"
	},

	/* International Telecommunication Limited (INTELCO) */
	{
		702, 68, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "INTELCO",
		"International Telecommunication Limited (INTELCO)"
	},

	/*********************
	 **** Guatemala ****
	 *********************/

	/* SERCOM S.A. (Guatemala) */
	{
		704, 1, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "PCS",
		"SERCOM S.A. (Guatemala)"
	},

	/* COMCEL-Communicaciones Celulares Sociedad Anonima */
	{
		704, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "COMCEL",
		"COMCEL GUATEMALA"
	},

	/* "Telefonica Moviles Guatamala, SA" */
	{
		704, 3, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "movistar",
		"Telefonica"
	},

	/*********************
	 **** El Salvador ****
	 *********************/

	/* CTE Telecom Personal SA de CV */
	{
		706, 1, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "ESV PERSONAL",
		"CTE Telecom Personal SA de CV"
	},

	/* DIGICEL, S.A. de C.V. */
	{
		706, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "Digicel",
		"DIGICEL, S.A. de C.V."
	},
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Telemovil EL Salvador S.A */
	{
		706, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "ESTELEMOVIL",
		"Telemovil EL Salvador S.A"
	},
#endif				/* FEATURE_SAMSUNG_BAND_GSM_850 */

	/* "Telefonica Moviles El Salvador, S.A de c.v" */
	{
		706, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "movistar",
		"Telefonica"
	},

	/* PERSONAL */
	{706, 10, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "PERSONAL", "PERSONAL"},	/* M625000000623 */

	/*********************
	 **** ************ ****
	 *********************/

	/* Megatel S.A. de C.V. */
	{
		708, 1, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "MEGATEL",
		"Megatel S.A. de C.V."
	},

	/* Telefonica Celular S.A (CELTEL) */
	{
		708, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CELTELHND",
		"Telefonica Celular S.A (CELTEL)"
	},

	/* Empresa Hondurena de Telecomunicaciones HONDUTEL */
	{
		708, 30, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "HT - 200",
		"Empresa Hondurena de Telecomunicaciones "
	},

	/*********************
	 **** Nicaragua ****
	 *********************/

	/* Empresa Nicaraguense de Telecomunicaciones S.A. - ENITEL */
	{710, 21, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "ENITEL", "ENITEL"},

	/* SERCOM S.A. (Nicaragua) */
	{
		710, 730, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "SERCOM",
		"SERCOM S.A. (Nicaragua)"
	},

	/*********************
	 **** ************ ****
	 *********************/

	/* Belize Telecommunications Ltd */
	{
		712, 1, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "I.C.E. ",
		"I.C.E. (Instituto Costarricense de Electricidad)"
	},

	/* I.C.E. (Instituto Costarricense de Electricidad) */
	{712, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "I.C.E.", "I.C.E. "},

	/*********************
	 **** Panama ****
	 *********************/
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Cable & Wireless Panama */
	{
		714, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "PANCW",
		"Cable & Wireless Panama"
	},

	/* "TELEFONICA MOVILES PANAMA, S.A." */
	{
		714, 20, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "movistar",
		"Movistar"
	},
#endif				/* FEATURE_SAMSUNG_BAND_GSM_850 */

	/**************
	 **** Peru ****
	 **************/

	/* Telefonica Moviles S.A. */
	{
		716, 6, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "movistar",
		"Movistar Peru"
	},

	/* TIM Peru S.A.C. */
	{
		716, 10, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "TIM",
		"TIM Peru S.A.C."
	},

	/*******************
	 **** Argentina ****
	 *******************/

	/* UNIFON */
	{722, 7, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "UNIFON", "UNIFON"},

	/* Telecom Personal S.A. */
	{
		722, 34, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "AR PERSONAL",
		"Telecom Personal S.A."
	},

	/* PORT-HABLE */
	{
		722, 35, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "PORT-HABLE",
		"PORT-HABLE"
	},

	/* CTI PCS S.A. */
	{
		722, 310, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "ARG CTI Movil",
		"CTI PCS S.A."
	},
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* CTI Compania de Telefonos del Interior S.A. */
	{
		722, 310, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "ARG CTI Movil",
		"CTI Compania de Telefonos del Interior S.A."
	},
#endif				/* FEATURE_SAMSUNG_BAND_GSM_850 */

	/***************
	 **** Brasil ****
	 ***************/

	/* TIM Celular S.A. */
	{724, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TIM", "TIM BRASIL"},

	/* TIM Celular S.A. */
	{724, 3, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TIM", "TIM BRASIL"},

	/* TIM Celular S.A. */
	{724, 4, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TIM", "TIM BRASIL"},

	/* Albra Telecommunicacoes Ltda */
	{724, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Claro", "Claro"},

	/* Americel S.A. */
	{724, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Claro", "Claro"},

	/* ATL - Algar Telecom Leste S.A. */
	{
		724, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Claro",
		"Claro - ATL"
	},

	/* BCP SA */
	{724, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Claro", "Claro "},

	/* BSE SA  */
	{724, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Claro", "Claro"},

	/* Stemar Telecommunicacoes Ltda */
	{724, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Claro", "Claro"},

	/* Telet S.A. */
	{724, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Claro", "Claro"},

	/* Tess S.A. */
	{724, 5, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Claro", "Claro"},

	/* Sercomtel Celular S/A */
	{724, 15, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SCTL", "SERCOMTEL"},

	/* Sercomtel Celular S/A */
	{724, 15, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SCTL", "SERCOMTEL"},

	/* 14 Brasil Telecom Celular S.A */
	{
		724, 16, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "BrTCel",
		"Brasil Telecom Celular "
	},

	/* Telemig Celular S/A */
	{
		724, 23, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TELEMIGC",
		"Telemig Celular S/A"
	},

	/* Telemig Celular S/A */
	{
		724, 23, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "TELEMIGC",
		"Telemig Celular S/A"
	},

	/* Amazonia Celular S/A */
	{
		724, 24, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "AMAZONIA",
		"Amazonia Celular S/A"
	},

	/* Amazonia Celular S/A */
	{
		724, 24, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "AMAZONIA",
		"Amazonia Celular S/A"
	},

	/* TNL PCS S.A. */
	{
		724, 31, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "Oi",
		"TNL PCS S.A."
	},

	/* Triangulo Celular S.A */
	{
		724, 32, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CTBC",
		"CTBC Cellular"
	},

	/* Triangulo Celular S.A */
	{
		724, 32, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "CTBC",
		"CTBC Cellular"
	},

	/* Triangulo Celular S.A */
	{
		724, 33, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CTBC",
		"CTBC Cellular"
	},

	/* Triangulo Celular S.A */
	{
		724, 33, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "CTBC",
		"CTBC Cellular"
	},

	/* Triangulo Celular S.A */
	{
		724, 34, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CTBC",
		"CTBC Cellular"
	},

	/* Triangulo Celular S.A */
	{
		724, 34, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "CTBC",
		"CTBC Cellular"
	},

	/***************
	 **** Chile ****
	 ***************/

	/* Entel Telefonia Movil S.A */
	{
		730, 1, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "CL ENTEL PCS",
		"Entel Telefonia Movil S.A"
	},

	/* Telefonica  Movil de Chile */
	{
		730, 2, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "TELEFONICA",
		"Telefonica  Movil de Chile"
	},

	/* Smartcom S.A. */
	{
		730, 3, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "SMARTCOM",
		"Smartcom"
	},

	/* Entel PCS Telecomunicaciones S.A */
	{
		730, 10, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "CL ENTEL PCS",
		"Entel PCS Telecomunicaciones S.A"
	},

	/*****************
	 **** Colombia ****
	 *****************/
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* Occidente y Caribe Celular SA Occel SA */
	{
		732, 101, SETTING_PREFERRED_NETWORK_TYPE_GSM_850,
		"COLOMBIA - COMCEL S.A", "Occidente y Caribe Celular SA Occel SA"
	},
#endif				/* FEATURE_SAMSUNG_BAND_GSM_850 */

	/* Comunicacion Celular SA Comcel SA */
	{
		732, 101, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "COMCEL",
		"Comunicacion Celular SA Comcel SA"
	},

	/* Colombia Movil SA */
	{
		732, 103, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "COL  MOVIL",
		"Colombia Movil SA"
	},

	/* Colombia Movil SA */
	{
		732, 111, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "OLA",
		"Colombia Movil SA"
	},

	/* Colombia Telecomunicaciones S.A. ESP */
	{
		732, 111, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "OLA",
		"Colombia Telecomunicaciones S.A. ESP"
	},

	/* Telefonica Moviles Colombia S.A. */
	{
		732, 123, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "COL_TEMC",
		"movistar"
	},

	/* Telefonica Moviles Colombia S.A. */
	{
		732, 123, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "COL_TEMC",
		"movistar"
	},

	/*******************
	   **** Venezuela ****
	 *******************/

	/* Infonet, Redes De Informacion C.A */
	{
		734, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "VZ INFO",
		"Infonet, Redes De Informacion C.A"
	},

	/* Corporacion Digitel C.A */
	{
		734, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "DIGITEL TIM",
		"Corporacion Digitel C.A"
	},

	/*****************
	 **** Bolivia ****
	 *****************/

	/* Nuevatel PCS De Bolivia SA */
	{
		736, 1, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "VIVA",
		"Nuevatel PCS De Bolivia SA"
	},

	/* Entel SA */
	{736, 2, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "BOMOV", "Entel SA"},

	/* Telefonica Celular De Bolivia S.A. */
	{
		736, 3, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "Telecel",
		"TELECEL BOLIVIA"
	},

	/**************
	 ****	  ****
	 **************/

	/* Cel*Star Guyana Inc */
	{
		738, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "TWTGUY",
		"Cel*Star Guyana Inc"
	},

	/* Guyana Telephone & Telegraph Co. */
	{
		738, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "CLNK PLS",
		"Cellink Plus"
	},

	/**************
	 **** ECU  ****
	 **************/
#ifdef FEATURE_SAMSUNG_BAND_GSM_850

	/* OTECEL S.A. */
	{
		740, 0, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "movistar",
		"MOVISTAR"
	},

	/* Conecel S.A. (Consorcio Ecuatoriano de Telecomunicaciones S.A.) */
	{
		740, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "PORTAGSM",
		"Conecel S.A. (Consorcio Ecuatoriano de Telecomunicaciones S.A.)"
	},
#endif				/* FEATURE_SAMSUNG_BAND_GSM_850 */

	/******************
	 **** Paraguay ****
	 ******************/

	/* Hola Paraguay S.A. */
	{744, 1, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "HPGYSA", "VOX"},

	/* AMX Paraguay Sociedad Anonima */
	{
		744, 2, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Porth",
		"AMX PARAGUAY S.A."
	},

	/* Telefonica Celular Del Paraguay S.A. (Telecel S.A.) */
	{
		744, 4, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "Telecel",
		"Telecel Paraguay"
	},

	/* Nucleo S.A */
	{
		744, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "Personal",
		"Personal"
	},

	/* Nucleo S.A */
	{
		744, 5, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Personal",
		"Personal"
	},

	/*****************
	 **** Surinam ****
	 *****************/

	/* NV ICMS */
	{746, 1, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "ICMS", "NV ICMS"},

	/* Telesur */
	{
		746, 2, SETTING_PREFERRED_NETWORK_TYPE_GSM_900, "SR.TELESUR.GSM",
		"Telesur"
	},

	/* NV ICMS */
	{
		746, 2, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "SR.TELESUR.GSM",
		"Telesur"
	},

	/*****************
	 ****		 ****
	 *****************/

	/* Abiatar S.A. */
	{
		748, 7, SETTING_PREFERRED_NETWORK_TYPE_GSM_850, "Movistar",
		"MOVISTAR"
	},

	/* Abiatar S.A. */
	{
		748, 7, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "Movistar",
		"MOVISTAR"
	},

	/* AM Wireless Uruguay S.A. */
	{
		748, 10, SETTING_PREFERRED_NETWORK_TYPE_PCS_1900, "CTIURY",
		"AM Wireless Uruguay S.A."
	},

	/*******************
	 **** Satellite ****
	 *******************/

	/* Thuraya Satellite Telecommunications Co */
	{
		901, 5, SETTING_PREFERRED_NETWORK_TYPE_GSM_SAT, "Thuraya",
		"Thuraya Satellite Telecommunications Co"
	},

	/* Maritime Communications Partner AS */
	{
		901, 12, SETTING_PREFERRED_NETWORK_TYPE_DCS_1800, "MCP",
		"Maritime Communications Partner AS"
	},

};				/*network_table */

static void setting_network_preferred_network_list_click_softkey_cancel(void
                                                                        *data,
                                                                        Evas_Object
                                                                        *obj,
                                                                        void
                                                                        *event_info);
static void setting_network_preferred_network_list_click_softkey_add(void *data,
                                                                     Evas_Object
                                                                     *obj, void
                                                                     *event_info);

/* static void setting_network_preferred_network_list_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info); */

#endif				/* _SETTINGS_NETWORK_PREFERRED_NETWORK_LIST_H_ */

/**
 *@}
 */
