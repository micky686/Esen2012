/***************************************************/
/*Modulname: bargraph.c                            */
/*Name: Florian Bartl                              */
/*MNr.: 0425594                                    */
/*Beschreibung:Treiber zur Ansteuerung der Bargraph*/
/*Datum:9.12.2011                                 */
/*                               */
/***************************************************/

#include "bargraph.h"


/********************************************************/
/*Prozedur: void init_bargraph(void)                  */
/*Beschreibung: Initialisiert die Bargraph.*/
/*Parameter: BARGRAPH_DDR: Data Direction Register auf Datenausgang einstellen*/
/*BARGRAPH_PORT: Ausgangstreiber auf high stellen. (LEDs sind lowaktiv)*/
/********************************************************/


void init_bargraph(void)
{
	BARGRAPH_DDR = 0xff;
	BARGRAPH_PORT = 0xff;
}

/**********************************************************************/
/*Prozedur: void set_bargraph(void)                                   */
/*Beschreibung: Die Bargraph gibt eine Dezimalzahl in Binaer aus.     */
/*Parameter:                                                          */
/*BARGRAPH_PORT: wird mit der Dezimalzahl belegt. (LEDs sind lowaktiv)*/
/**********************************************************************/


void set_bargraph(uint8_t value)
{
	BARGRAPH_PORT = ~value;
}

