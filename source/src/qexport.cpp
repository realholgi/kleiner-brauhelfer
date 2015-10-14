#include "qexport.h"
#include "errormessage.h"
#include "definitionen.h"
#include "rohstoffaustauschen.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDomDocument>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QVariant>
#include <QDateTime>
#include <QPushButton>
#include <QCoreApplication>
//
QExport::QExport(  ) 
	: QObject()
{
	// TODO
}
//

//Exportiert einen Sud in eine XML Datei
int QExport::ExportSudXML(int SudNr, QString Dateiname)
{
	//Sud aus Datenbank abfragen
	QSqlQuery query_sud;
	int FeldNr;
	QString sql = "SELECT * FROM Sud WHERE ID=" + QString::number(SudNr) + ";";
	if (!query_sud.exec(sql)) {
		// Fehlermeldung Datenbankabfrage
		ErrorMessage *errorMessage = new ErrorMessage();
		errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
			CANCEL_NO, trUtf8("Rueckgabe:\n") + query_sud.lastError().databaseText()
			+ trUtf8("\nSQL Befehl:\n") + sql);
	}
	else {
		if (query_sud.first()) {
			QDomText text;
			QDomElement element;
			QDomComment komentar;
			//Daten in XML Datei schreiben
			QDomDocument doc("");

			//Wurzelelement
			QDomElement daten = doc.createElement("xsud");
			doc.appendChild(daten);

			//Global
			QDomElement global = doc.createElement("Global");
			daten.appendChild(global);
			//Erstelldatum der Sud export Datei
			komentar = doc.createComment("Erstellungsdatum der Exportdatei");
			global.appendChild(komentar);
			element = doc.createElement("Datum");
			text = doc.createTextNode(QDateTime::currentDateTime().toString(Qt::ISODate));
			element.setAttribute("Einheit","Qt::ISODate");
			element.appendChild(text);
			global.appendChild(element);

			//Versionen
			QDomElement version = doc.createElement("Version");
			daten.appendChild(version);
			
			komentar = doc.createComment("Versionsstand der Datenbank");
			version.appendChild(komentar);
			element = doc.createElement("Datenbank");
			text = doc.createTextNode(QString::number(DB_VERSION));
			element.appendChild(text);
			version.appendChild(element);
			
			komentar = doc.createComment("Versionsstand der Exportdatei");
			version.appendChild(komentar);
			element = doc.createElement("xsud");
			text = doc.createTextNode(XSUD_VERSION);
			element.appendChild(text);
			version.appendChild(element);

			komentar = doc.createComment("Versionsstand kleiner-brauhelfer");
			version.appendChild(komentar);
			element = doc.createElement("kleiner-brauhelfer");
			text = doc.createTextNode(VERSION);
			element.appendChild(text);
			version.appendChild(element);

			QDomElement sud = doc.createElement("Sud");
			//Sudname
			komentar = doc.createComment("Bezeichner fuer den Sud");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Sudname");
			element = doc.createElement("Sudname");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Erstellungsdatum
			komentar = doc.createComment("Zeitstempel fuer Erstellungsdatum in der Datenbank");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Erstellt");
			element = doc.createElement("Erstellt");
			element.setAttribute("Einheit","Qt::ISODate");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Zuletzt Gespeichert
			komentar = doc.createComment("Zeitstempel wann Sud das letzte-mal in der Datenbank gespeichert wurde");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Gespeichert");
			element = doc.createElement("Gespeichert");
			element.setAttribute("Einheit","Qt::ISODate");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Menge
			komentar = doc.createComment("Soll Wuerze-Anstellmenge");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Menge");
			element = doc.createElement("Menge");
			element.setAttribute("Einheit","Liter");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Stammwuerze
			komentar = doc.createComment("Soll Stammwuerze");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("SW");
			element = doc.createElement("SW");
			element.setAttribute("Einheit","Grad Plato");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//CO2 Gehalt
			komentar = doc.createComment("Soll CO2 Gehalt");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("CO2");
			element = doc.createElement("CO2");
			element.setAttribute("Einheit","Gramm/Liter");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Bitterkeit
			komentar = doc.createComment("Soll Bittere");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("IBU");
			element = doc.createElement("IBU");
			element.setAttribute("Einheit","IBU");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Reifezeit
			komentar = doc.createComment("Angepeilte Reifezeit");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Reifezeit");
			element = doc.createElement("Reifezeit");
			element.setAttribute("Einheit","Wochen");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Kommentar
			komentar = doc.createComment("Allgemeiner Kommentar");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Kommentar");
			element = doc.createElement("Kommentar");
			//element.setAttribute("Einheit","Liter");
			QString s = query_sud.value(FeldNr).toString();
			//Alle Zeilenumbrueche gegen <br> austauschen
			s.replace('\n',"<br>");
			text = doc.createCDATASection(s);
			element.appendChild(text);
			sud.appendChild(element);
			
			//Braudatum
			komentar = doc.createComment("Datum an dem der Sud gebraut wurde");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Braudatum");
			element = doc.createElement("Braudatum");
			element.setAttribute("Einheit","Qt::ISODate");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Flag ob Bier gebraut wurde
			komentar = doc.createComment("Flag ob Sud gebraut wurde");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("BierWurdeGebraut");
			element = doc.createElement("BierWurdeGebraut");
			element.setAttribute("Einheit","Bool");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			bool BierWurdeGebraut = query_sud.value(FeldNr).toBool();
			element.appendChild(text);
			sud.appendChild(element);
			
			//Flag ob Bier abgefuellt wurde
			komentar = doc.createComment("Flag ob Sud abgefuellt wurde");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("BierWurdeAbgefuellt");
			element = doc.createElement("BierWurdeAbgefuellt");
			element.setAttribute("Einheit","Bool");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Flag ob Bier verbraucht wurde
			komentar = doc.createComment("Flag ob Sud verbraucht wurde");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("BierWurdeVerbraucht");
			element = doc.createElement("BierWurdeVerbraucht");
			element.setAttribute("Einheit","Bool");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Kochdauer Nach Bitterhopfengabe
			komentar = doc.createComment("Kochdauer nach der ersten Hopfengabe");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("KochdauerNachBitterhopfung");
			element = doc.createElement("KochdauerNachBitterhopfung");
			element.setAttribute("Einheit","Minuten");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Faktor fuer Hauptguss
			komentar = doc.createComment("Faktor zum Berechnen der Hauptgussmenge (Schuettung * Faktor = Hauptgussmenge)");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("FaktorHauptguss");
			element = doc.createElement("FaktorHauptguss");
			element.setAttribute("Einheit","Faktor");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Auswahl Hefe
			komentar = doc.createComment("Name der Ausgewaehlten Hefe");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("AuswahlHefe");
			element = doc.createElement("AuswahlHefe");
			element.setAttribute("Einheit","Text");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			QString AuswahlHefe = query_sud.value(FeldNr).toString();
			element.appendChild(text);
			sud.appendChild(element);

			//Anzahl verwendeter Hefe Einheiten
			komentar = doc.createComment("Anzahl verwendeter Hefe Einheiten");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("HefeAnzahlEinheiten");
			element = doc.createElement("HefeAnzahlEinheiten");
			element.setAttribute("Einheit","Integer");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Vor dem Hopfenseihen
			//Wuerzemenge vor dem Hopfenseihen
			komentar = doc.createComment("Wuerzemenge vor dem Hopfenseihen");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("WuerzemengeVorHopfenseihen");
			element = doc.createElement("WuerzemengeVorHopfenseihen");
			element.setAttribute("Einheit","Liter");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Stammwuerze vor dem Hopfenseihen
			komentar = doc.createComment("Stammwuerze vor dem Hopfenseihen");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("SWVorHopfenseihen");
			element = doc.createElement("SWVorHopfenseihen");
			element.setAttribute("Einheit","Grad Plato");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Nach dem Hopfenseihen
			//Wuerzemenge nach dem Hopfenseihen
			komentar = doc.createComment("Wuerzemenge nach dem Hopfenseihen");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("WuerzemengeKochende");
			element = doc.createElement("WuerzemengeKochende");
			element.setAttribute("Einheit","Liter");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Stammwuerze nach dem Hopfenseihen
			komentar = doc.createComment("Stammwuerze nach dem Hopfenseihen");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("SWKochende");
			element = doc.createElement("SWKochende");
			element.setAttribute("Einheit","Grad Plato");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Abgezwackte Speisemenge
			komentar = doc.createComment("Abgefuellte Speisemenge");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Speisemenge");
			element = doc.createElement("Speisemenge");
			element.setAttribute("Einheit","Liter");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
		
			//Vor der Hefezugabe
			//Datum der Hefezugabe
			komentar = doc.createComment("Datum der Hefezugabe");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Anstelldatum");
			element = doc.createElement("Anstelldatum");
			element.setAttribute("Einheit","Qt::ISODate");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Wuerzemenge beim Anstellen
			komentar = doc.createComment("Wuerzemenge beim der Hefezugabe");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("WuerzemengeAnstellen");
			element = doc.createElement("WuerzemengeAnstellen");
			element.setAttribute("Einheit","Liter");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Stammwuerze beim Anstellen
			komentar = doc.createComment("Stammwuerze bei der Hefezugabe");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("SWAnstellen");
			element = doc.createElement("SWAnstellen");
			element.setAttribute("Einheit","Grad Plato");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
		
			//Beim Abfuellen
			//Abfuelldatum
			komentar = doc.createComment("Abfuelldatum");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Abfuelldatum");
			element = doc.createElement("Abfuelldatum");
			element.setAttribute("Einheit","Qt::ISODate");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Stammwuerze der Schnellgaerprobe
			komentar = doc.createComment("Restextrakt der Schnellgaerprobe");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("SWSchnellgaerprobe");
			element = doc.createElement("SWSchnellgaerprobe");
			element.setAttribute("Einheit","Grad Plato");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Stammwuerze Jungbier
			komentar = doc.createComment("Restextrakt des Jungbieres aus der Hauptgaerung");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("SWJungbier");
			element = doc.createElement("SWJungbier");
			element.setAttribute("Einheit","Grad Plato");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Temperatur Jungbier beim Abfuellen
			komentar = doc.createComment("Temperatur Jungbier beim Abfuellen");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("TemperaturJungbier");
			element = doc.createElement("TemperaturJungbier");
			element.setAttribute("Einheit","Grad Celsius");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Temperatur Einmaischen
			komentar = doc.createComment("Temperatur Einmaischen");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("EinmaischenTemp");
			element = doc.createElement("EinmaischenTemp");
			element.setAttribute("Einheit","Grad Celsius");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);
			
			//Kosten Wasser Strom
			komentar = doc.createComment("Kosten fuer Wasser / Strom / Gas etc.");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("KostenWasserStrom");
			element = doc.createElement("KostenWasserStrom");
			element.setAttribute("Einheit","Euro");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Nachisomerisierungszeit
			komentar = doc.createComment("Dauer die der Hopfen noch nach dem Kochen Bittere abgeben kann");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Nachisomerisierungszeit");
			element = doc.createElement("Nachisomerisierungszeit");
			element.setAttribute("Einheit","Minuten");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);


			//Gespeichertes Tab wiederherstellen
			komentar = doc.createComment("Aktives Tab im kleinen-brauhelfer nach dem Laden");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("AktivTab");
			element = doc.createElement("AktivTab");
			element.setAttribute("Einheit","Integer");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Gespeichertes Tab im Gaerverlauf wiederherstellen
			komentar = doc.createComment("Aktives Tab im Gaerverlauf nach dem Laden");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("AktivTab_Gaerverlauf");
			element = doc.createElement("AktivTab_Gaerverlauf");
			element.setAttribute("Einheit","Integer");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Gewünschte Restalkalität
			komentar = doc.createComment("Gewuenschte Restalkalitaet");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("RestalkalitaetSoll");
			element = doc.createElement("RestalkalitaetSoll");
			element.setAttribute("Einheit","Grad Deutsche Haerte");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Flag ob Schnellgärprobe aktiv ist
			komentar = doc.createComment("Flag ob eine Schnellgaeprobe gemacht wurde");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("SchnellgaerprobeAktiv");
			element = doc.createElement("SchnellgaerprobeAktiv");
			element.setAttribute("Einheit","Bool");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Farbe
			komentar = doc.createComment("Bierfarbe");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("erg_Farbe");
			element = doc.createElement("erg_Farbe");
			element.setAttribute("Einheit","EBC");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Jungbiermenge beim Abfüllen
			komentar = doc.createComment("Jungbiermenge beim Abfuellen");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("JungbiermengeAbfuellen");
			element = doc.createElement("JungbiermengeAbfuellen");
			element.setAttribute("Einheit","Liter");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Bewertung Anzahl Sterne
			komentar = doc.createComment("Beste Bewertung (Anzahl Sterne)");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("Bewertung");
			element = doc.createElement("Bewertung");
			element.setAttribute("Einheit","Integer");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Reifewoche der Besten Bewertung
			komentar = doc.createComment("Reifewoche der Besten Bewertung");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("BewertungText");
			element = doc.createElement("BewertungText");
			element.setAttribute("Einheit","Text");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Maximale Anzahl Sterne bei diesem Sud
			komentar = doc.createComment("Maximale Anzahl Sterne bei diesem Sud");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("BewertungMaxSterne");
			element = doc.createElement("BewertungMaxSterne");
			element.setAttribute("Einheit","Integer");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Art der Hopfenberechnung
			komentar = doc.createComment("Art der Hopfenberechnung");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("berechnungsArtHopfen");
			element = doc.createElement("berechnungsArtHopfen");
			element.setAttribute("Einheit","Integer");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//High Gravity Faktor
			komentar = doc.createComment("High Gravity Faktor");
			sud.appendChild(komentar);
			FeldNr = query_sud.record().indexOf("highGravityFaktor");
			element = doc.createElement("highGravityFaktor");
			element.setAttribute("Einheit","Integer");
			text = doc.createTextNode(query_sud.value(FeldNr).toString());
			element.appendChild(text);
			sud.appendChild(element);

			//Ergebnisse
			if (BierWurdeGebraut) {
				//Menge Gesammtschuettung
				komentar = doc.createComment("Gesammtmenge Schuettung");
				sud.appendChild(komentar);
				FeldNr = query_sud.record().indexOf("erg_S_Gesammt");
				element = doc.createElement("erg_S_Gesammt");
				element.setAttribute("Einheit","kg");
				text = doc.createTextNode(query_sud.value(FeldNr).toString());
				element.appendChild(text);
				sud.appendChild(element);

				//Wasser Gesammt
				komentar = doc.createComment("Gesammtmenge Wasser");
				sud.appendChild(komentar);
				FeldNr = query_sud.record().indexOf("erg_W_Gesammt");
				element = doc.createElement("erg_W_Gesammt");
				element.setAttribute("Einheit","Liter");
				text = doc.createTextNode(query_sud.value(FeldNr).toString());
				element.appendChild(text);
				sud.appendChild(element);

				//Wasser Hauptguss
				komentar = doc.createComment("Menge Hauptguss");
				sud.appendChild(komentar);
				FeldNr = query_sud.record().indexOf("erg_WHauptguss");
				element = doc.createElement("erg_WHauptguss");
				element.setAttribute("Einheit","Liter");
				text = doc.createTextNode(query_sud.value(FeldNr).toString());
				element.appendChild(text);
				sud.appendChild(element);

				//Wasser Nachguss
				komentar = doc.createComment("Menge Nachguss");
				sud.appendChild(komentar);
				FeldNr = query_sud.record().indexOf("erg_WNachguss");
				element = doc.createElement("erg_WNachguss");
				element.setAttribute("Einheit","Liter");
				text = doc.createTextNode(query_sud.value(FeldNr).toString());
				element.appendChild(text);
				sud.appendChild(element);

				//Sudhausausbeute
				komentar = doc.createComment("Sudhausausbeute");
				sud.appendChild(komentar);
				FeldNr = query_sud.record().indexOf("erg_Sudhausausbeute");
				element = doc.createElement("erg_Sudhausausbeute");
				element.setAttribute("Einheit","Prozent");
				text = doc.createTextNode(query_sud.value(FeldNr).toString());
				element.appendChild(text);
				sud.appendChild(element);

				//Kosten Pro Liter
				komentar = doc.createComment("Kosten pro Liter Bier");
				sud.appendChild(komentar);
				FeldNr = query_sud.record().indexOf("erg_Preis");
				element = doc.createElement("erg_Preis");
				element.setAttribute("Einheit","Euro/Liter");
				text = doc.createTextNode(query_sud.value(FeldNr).toString());
				element.appendChild(text);
				sud.appendChild(element);

				//Alkoholgehalt
				komentar = doc.createComment("Alkoholgehalt");
				sud.appendChild(komentar);
				FeldNr = query_sud.record().indexOf("erg_Alkohol");
				element = doc.createElement("erg_Alkohol");
				element.setAttribute("Einheit","vol. Prozent");
				text = doc.createTextNode(query_sud.value(FeldNr).toString());
				element.appendChild(text);
				sud.appendChild(element);

				//Effektive Ausbeute
				komentar = doc.createComment("Effektive Ausbeute von diesem Sud");
				sud.appendChild(komentar);
				FeldNr = query_sud.record().indexOf("erg_EffektiveAusbeute");
				element = doc.createElement("erg_EffektiveAusbeute");
				element.setAttribute("Einheit","Prozent");
				text = doc.createTextNode(query_sud.value(FeldNr).toString());
				element.appendChild(text);
				sud.appendChild(element);

				//Abgefuellte Biermenge
				komentar = doc.createComment("Abgefuellte Biermenge");
				sud.appendChild(komentar);
				FeldNr = query_sud.record().indexOf("erg_AbgefuellteBiermenge");
				element = doc.createElement("erg_AbgefuellteBiermenge");
				element.setAttribute("Einheit","Liter");
				text = doc.createTextNode(query_sud.value(FeldNr).toString());
				element.appendChild(text);
				sud.appendChild(element);
			}

			//Rasten Abfragen
			QSqlQuery query_rasten;
			sql = "SELECT * FROM Rasten WHERE SudID=" + QString::number(SudNr) + ";";
			if (!query_rasten.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rueckgabe:\n") + query_rasten.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				QDomElement Rasten = doc.createElement("Rasten");
				sud.appendChild(Rasten);
				QDomElement Rast;

				int i = 1;
				while (query_rasten.next()){
					//Alle Rasten einlesen
					Rast = doc.createElement("Rast_" + QString::number(i));
					Rasten.appendChild(Rast);
					
					//Rast aktiv
					komentar = doc.createComment("1 = Rast ist Aktiv");
					Rast.appendChild(komentar);
					FeldNr = query_rasten.record().indexOf("RastAktiv");
					element = doc.createElement("RastAktiv");
					element.setAttribute("Einheit","Bool");
					text = doc.createTextNode(query_rasten.value(FeldNr).toString());
					element.appendChild(text);
					Rast.appendChild(element);

					//Rast Name
					komentar = doc.createComment("Berschreibung der Rast");
					Rast.appendChild(komentar);
					FeldNr = query_rasten.record().indexOf("RastName");
					element = doc.createElement("RastName");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query_rasten.value(FeldNr).toString());
					element.appendChild(text);
					Rast.appendChild(element);

					//Rast Temperatur
					komentar = doc.createComment("Temperatur der Rast");
					Rast.appendChild(komentar);
					FeldNr = query_rasten.record().indexOf("RastTemp");
					element = doc.createElement("RastTemp");
					element.setAttribute("Einheit","Grad Celsius");
					text = doc.createTextNode(query_rasten.value(FeldNr).toString());
					element.appendChild(text);
					Rast.appendChild(element);

					//Rast Dauer
					komentar = doc.createComment("Rastdauer");
					Rast.appendChild(komentar);
					FeldNr = query_rasten.record().indexOf("RastDauer");
					element = doc.createElement("RastDauer");
					element.setAttribute("Einheit","Minuten");
					text = doc.createTextNode(query_rasten.value(FeldNr).toString());
					element.appendChild(text);
					Rast.appendChild(element);

					i++;
				}
			}
			//Schuettung Abfragen
			QSqlQuery query_Malz;
			sql = "SELECT * FROM Malzschuettung WHERE SudID=" + QString::number(SudNr) + ";";
			if (!query_Malz.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rueckgabe:\n") + query_Malz.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				QDomElement Schuettung = doc.createElement("Schuettung");
				sud.appendChild(Schuettung);
				QDomElement Anteil;

				int i = 1;
				while (query_Malz.next()){
					Anteil = doc.createElement("Anteil_" + QString::number(i));
					Schuettung.appendChild(Anteil);
					
					//Name
					komentar = doc.createComment("Malzbeschreibung (Name)");
					Anteil.appendChild(komentar);
					FeldNr = query_Malz.record().indexOf("Name");
					element = doc.createElement("Name");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query_Malz.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Prozent
					komentar = doc.createComment("Prozentualer Anteil der Schuettung");
					Anteil.appendChild(komentar);
					FeldNr = query_Malz.record().indexOf("Prozent");
					element = doc.createElement("Prozent");
					element.setAttribute("Einheit","Prozent");
					text = doc.createTextNode(query_Malz.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//erg_Menge
					komentar = doc.createComment("Berechneter Gewichtsanteil der Schuettung");
					Anteil.appendChild(komentar);
					FeldNr = query_Malz.record().indexOf("erg_Menge");
					element = doc.createElement("erg_Menge");
					element.setAttribute("Einheit","Kg");
					text = doc.createTextNode(query_Malz.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Farbe
					komentar = doc.createComment("Malz - Farbwert");
					Anteil.appendChild(komentar);
					FeldNr = query_Malz.record().indexOf("Farbe");
					element = doc.createElement("Farbe");
					element.setAttribute("Einheit","EBC");
					text = doc.createTextNode(query_Malz.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					i++;
				}
			}
			//Hopfengaben Abfragen
			QSqlQuery query_Hopfen;
			sql = "SELECT * FROM Hopfengaben WHERE SudID=" + QString::number(SudNr) + ";";
			if (!query_Hopfen.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rueckgabe:\n") + query_Hopfen.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				QDomElement Hopfengaben = doc.createElement("Hopfengaben");
				sud.appendChild(Hopfengaben);
				QDomElement Anteil;

				int i = 1;
				while (query_Hopfen.next()){
					Anteil = doc.createElement("Anteil_" + QString::number(i));
					Hopfengaben.appendChild(Anteil);
					
					//Aktiv
					komentar = doc.createComment("1 = Hopfengabe Aktiv");
					Anteil.appendChild(komentar);
					FeldNr = query_Hopfen.record().indexOf("Aktiv");
					element = doc.createElement("Aktiv");
					element.setAttribute("Einheit","Bool");
					text = doc.createTextNode(query_Hopfen.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Name
					komentar = doc.createComment("Berschreibung Hopfen (Name)");
					Anteil.appendChild(komentar);
					FeldNr = query_Hopfen.record().indexOf("Name");
					element = doc.createElement("Name");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query_Hopfen.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Zeit
					komentar = doc.createComment("Zeit nach Hopfengabe 1");
					Anteil.appendChild(komentar);
					FeldNr = query_Hopfen.record().indexOf("Zeit");
					element = doc.createElement("Zeit");
					element.setAttribute("Einheit","Minuten");
					text = doc.createTextNode(query_Hopfen.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Prozent
					komentar = doc.createComment("Prozentualer Gewichts-anteil der Hopfengabe");
					Anteil.appendChild(komentar);
					FeldNr = query_Hopfen.record().indexOf("Prozent");
					element = doc.createElement("Prozent");
					element.setAttribute("Einheit","Prozent");
					text = doc.createTextNode(query_Hopfen.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//erg_Menge
					komentar = doc.createComment("Berechnete Gewichtsmenge");
					Anteil.appendChild(komentar);
					FeldNr = query_Hopfen.record().indexOf("erg_Menge");
					element = doc.createElement("erg_Menge");
					element.setAttribute("Einheit","Gramm");
					text = doc.createTextNode(query_Hopfen.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//erg_Hopfentext
					komentar = doc.createComment("Beschreibungstext");
					Anteil.appendChild(komentar);
					FeldNr = query_Hopfen.record().indexOf("erg_Hopfentext");
					element = doc.createElement("erg_Hopfentext");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query_Hopfen.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Alpha
					komentar = doc.createComment("Alphaprozent gehalt des Hopfens");
					Anteil.appendChild(komentar);
					FeldNr = query_Hopfen.record().indexOf("Alpha");
					element = doc.createElement("Alpha");
					element.setAttribute("Einheit","Alpha Prozent");
					text = doc.createTextNode(query_Hopfen.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Pellets
					komentar = doc.createComment("Hopfenart 1 = Pellets");
					Anteil.appendChild(komentar);
					FeldNr = query_Hopfen.record().indexOf("Pellets");
					element = doc.createElement("Pellets");
					element.setAttribute("Einheit","Bool");
					text = doc.createTextNode(QString::number(query_Hopfen.value(FeldNr).toBool()));
					element.appendChild(text);
					Anteil.appendChild(element);

					//Vorderwürze
					komentar = doc.createComment("Wen diese Gabe eine Vorderwuerzehopfung ist dann = 1");
					Anteil.appendChild(komentar);
					FeldNr = query_Hopfen.record().indexOf("Vorderwuerze");
					element = doc.createElement("Vorderwuerze");
					element.setAttribute("Einheit","Bool");
					text = doc.createTextNode(QString::number(query_Hopfen.value(FeldNr).toBool()));
					element.appendChild(text);
					Anteil.appendChild(element);

					i++;
				}
			}

			//Weitere Zutaten abfragen
			QSqlQuery query_WeitereZutaten;
			sql = "SELECT * FROM WeitereZutatenGaben WHERE SudID=" + QString::number(SudNr) + ";";
			if (!query_WeitereZutaten.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rueckgabe:\n") + query_WeitereZutaten.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				QDomElement WeitereZutatenGaben = doc.createElement("WeitereZutatenGaben");
				sud.appendChild(WeitereZutatenGaben);
				QDomElement Anteil;

				int i = 1;
				while (query_WeitereZutaten.next()){
					Anteil = doc.createElement("Anteil_" + QString::number(i));
					WeitereZutatenGaben.appendChild(Anteil);

					//Name
					komentar = doc.createComment("Name der Zutat (Name)");
					Anteil.appendChild(komentar);
					FeldNr = query_WeitereZutaten.record().indexOf("Name");
					element = doc.createElement("Name");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query_WeitereZutaten.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Menge
					komentar = doc.createComment("Menge");
					Anteil.appendChild(komentar);
					FeldNr = query_WeitereZutaten.record().indexOf("Menge");
					element = doc.createElement("Menge");
					element.setAttribute("Einheit","Gramm/Liter");
					text = doc.createTextNode(query_WeitereZutaten.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Einheit
					komentar = doc.createComment("Einheiten Typ 0=Kg 1=g");
					Anteil.appendChild(komentar);
					FeldNr = query_WeitereZutaten.record().indexOf("Einheit");
					element = doc.createElement("Einheit");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query_WeitereZutaten.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Typ
					komentar = doc.createComment("Art der Zutat 0=Honig 1=Zucker 2=Gewuerz 3=Frucht 4=Sonstiges 100=Hopfen");
					Anteil.appendChild(komentar);
					FeldNr = query_WeitereZutaten.record().indexOf("Typ");
					element = doc.createElement("Typ");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query_WeitereZutaten.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Zeitpunkt
					komentar = doc.createComment("Zeitpunkt der Zugabe 0=Gaerung 1=Anstellen 2=Kochbeginn 3=Maischen");
					Anteil.appendChild(komentar);
					FeldNr = query_WeitereZutaten.record().indexOf("Zeitpunkt");
					element = doc.createElement("Zeitpunkt");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query_WeitereZutaten.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Bemerkung
					komentar = doc.createComment("Bemerkung");
					Anteil.appendChild(komentar);
					FeldNr = query_WeitereZutaten.record().indexOf("Bemerkung");
					element = doc.createElement("Bemerkung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query_WeitereZutaten.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Ergebniss Menge
					komentar = doc.createComment("Berechnete Menge");
					Anteil.appendChild(komentar);
					FeldNr = query_WeitereZutaten.record().indexOf("erg_Menge");
					element = doc.createElement("erg_Menge");
					element.setAttribute("Einheit","Gramm");
					text = doc.createTextNode(query_WeitereZutaten.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Ausbeute (Anzeil der Stammwürzenerhöhung)
					komentar = doc.createComment("Ausbeute 0= Keine Auswirkung auf die Stammwürze");
					Anteil.appendChild(komentar);
					FeldNr = query_WeitereZutaten.record().indexOf("Ausbeute");
					element = doc.createElement("Ausbeute");
					element.setAttribute("Einheit","Prozent");
					text = doc.createTextNode(query_WeitereZutaten.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					//Farbe
					komentar = doc.createComment("Farbe 0=Keine Auswirkung auf die Farbe");
					Anteil.appendChild(komentar);
					FeldNr = query_WeitereZutaten.record().indexOf("Farbe");
					element = doc.createElement("Farbe");
					element.setAttribute("Einheit","EBC");
					text = doc.createTextNode(query_WeitereZutaten.value(FeldNr).toString());
					element.appendChild(text);
					Anteil.appendChild(element);

					i++;
				}
			}


			//Gärverlauf Schnellgärprobe
			QSqlQuery query;
			sql = "SELECT * FROM Schnellgaerverlauf WHERE SudID=" + QString::number(SudNr) + " ORDER BY Zeitstempel;";
			if (!query.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				QDomElement Schnellgaerverlauf = doc.createElement("Schnellgaerverlauf");
				sud.appendChild(Schnellgaerverlauf);
				QDomElement Eintrag;

				int i=0;
				while (query.next()){
					Eintrag = doc.createElement("Eintrag_" + QString::number(i + 1));
					Schnellgaerverlauf.appendChild(Eintrag);

					//Zeitstempel
					komentar = doc.createComment("Zeitstempel der Messung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Zeitstempel");
					element = doc.createElement("Zeitstempel");
					element.setAttribute("Einheit","Qt::ISODate");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Stammwürze
					komentar = doc.createComment("Restextrakt der Messung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("SW");
					element = doc.createElement("SW");
					element.setAttribute("Einheit","Grad Plato");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Alkoholgehalt
					komentar = doc.createComment("Berechneter Alkoholgehalt der Messung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Alc");
					element = doc.createElement("Alc");
					element.setAttribute("Einheit","vol. Prozent");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Temperatur
					komentar = doc.createComment("Temperatur der Schnellgaerprobe bei der Messung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Temp");
					element = doc.createElement("Temp");
					element.setAttribute("Einheit","Grad Celsius");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					i++;
				}
			}
			
			//Gärverlauf Hauptgärung
			QString sql = "SELECT * FROM Hauptgaerverlauf WHERE SudID=" + QString::number(SudNr) + " ORDER BY Zeitstempel;";
			if (!query.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				QDomElement Hauptgaerverlauf = doc.createElement("Hauptgaerverlauf");
				sud.appendChild(Hauptgaerverlauf);
				QDomElement Eintrag;

				int i=0;
				while (query.next()){
					Eintrag = doc.createElement("Eintrag_" + QString::number(i + 1));
					Hauptgaerverlauf.appendChild(Eintrag);

					//Zeitstempel
					komentar = doc.createComment("Zeitstempel der Messung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Zeitstempel");
					element = doc.createElement("Zeitstempel");
					element.setAttribute("Einheit","Qt::ISODate");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Stammwürze
					komentar = doc.createComment("Restextrakt der Messung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("SW");
					element = doc.createElement("SW");
					element.setAttribute("Einheit","Grad Plato");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Alkoholgehalt
					komentar = doc.createComment("Berechneter Alkoholgehalt der Messung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Alc");
					element = doc.createElement("Alc");
					element.setAttribute("Einheit","vol. Prozent");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Temperatur
					komentar = doc.createComment("Temperatur der Hauptgaerung bei der Messung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Temp");
					element = doc.createElement("Temp");
					element.setAttribute("Einheit","Grad Celsius");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					i++;
				}
			}

			//Tabelle einlesen
			sql = "SELECT * FROM Nachgaerverlauf WHERE SudID=" + QString::number(SudNr) + " ORDER BY Zeitstempel;";
			if (!query.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				QDomElement Nachgaerverlauf = doc.createElement("Nachgaerverlauf");
				sud.appendChild(Nachgaerverlauf);
				QDomElement Eintrag;

				int i=0;
				while (query.next()){
					Eintrag = doc.createElement("Eintrag_" + QString::number(i + 1));
					Nachgaerverlauf.appendChild(Eintrag);

					//Zeitstempel
					komentar = doc.createComment("Zeitstempel der Messung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Zeitstempel");
					element = doc.createElement("Zeitstempel");
					element.setAttribute("Einheit","Qt::ISODate");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Druck
					komentar = doc.createComment("Flaschen / Fassdruck der Messung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Druck");
					element = doc.createElement("Druck");
					element.setAttribute("Einheit","Bar");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Temperatur
					komentar = doc.createComment("Temperatur des Jungbieres");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Temp");
					element = doc.createElement("Temp");
					element.setAttribute("Einheit","Grad Celsius");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Co2 Gehalt
					komentar = doc.createComment("Berechneter CO2 Gehalt des Jungbieres");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("CO2");
					element = doc.createElement("CO2");
					element.setAttribute("Einheit","Gramm/Liter");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					i++;
				}
			}
			daten.appendChild(sud);

			//Bewertungen
			sql = "SELECT * FROM Bewertungen WHERE SudID=" + QString::number(SudNr) + " ORDER BY Datum;";
			if (!query.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				QDomElement Bewertungen = doc.createElement("Bewertungen");
				sud.appendChild(Bewertungen);
				QDomElement Eintrag;

				int i=0;
				while (query.next()){
					Eintrag = doc.createElement("Eintrag_" + QString::number(i + 1));
					Bewertungen.appendChild(Eintrag);

					//Woche
					komentar = doc.createComment("Wievielte Reifewoche");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Woche");
					element = doc.createElement("Woche");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Datum
					komentar = doc.createComment("Datum der Bewertung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Datum");
					element = doc.createElement("Datum");
					element.setAttribute("Einheit","Qt::ISODate");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Sterne
					komentar = doc.createComment("Anzahl Sterne");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Sterne");
					element = doc.createElement("Sterne");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Bemerkung
					komentar = doc.createComment("Globale Bemerkung in der Bewertung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Bemerkung");
					element = doc.createElement("Bemerkung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Farbe
					komentar = doc.createComment("Verkostungseinstellung Farbe binaer codiert");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Farbe");
					element = doc.createElement("Farbe");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Farbe Bemerkung
					komentar = doc.createComment("Bemerkung zur Farbe");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("FarbeBemerkung");
					element = doc.createElement("FarbeBemerkung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Schaum
					komentar = doc.createComment("Verkostungseinstellung Schaum binaer codiert");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Schaum");
					element = doc.createElement("Schaum");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Schaum Bemerkung
					komentar = doc.createComment("Bemerkung zum Schaum");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("SchaumBemerkung");
					element = doc.createElement("SchaumBemerkung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Geruch
					komentar = doc.createComment("Verkostungseinstellung Geruch binaer codiert");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Geruch");
					element = doc.createElement("Geruch");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Geruch Bemerkung
					komentar = doc.createComment("Bemerkung zum Geruch");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("GeruchBemerkung");
					element = doc.createElement("GeruchBemerkung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Geschmack
					komentar = doc.createComment("Verkostungseinstellung Geschmack binaer codiert");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Geschmack");
					element = doc.createElement("Geschmack");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Geschmack Bemerkung
					komentar = doc.createComment("Bemerkung zum Geschmack");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("GeschmackBemerkung");
					element = doc.createElement("GeschmackBemerkung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Antrunk
					komentar = doc.createComment("Verkostungseinstellung Antrunk binaer codiert");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Antrunk");
					element = doc.createElement("Antrunk");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Antrunk Bemerkung
					komentar = doc.createComment("Bemerkung zum Antrunk");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("AntrunkBemerkung");
					element = doc.createElement("AntrunkBemerkung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Haupttrunk
					komentar = doc.createComment("Verkostungseinstellung Haupttrunk binaer codiert");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Haupttrunk");
					element = doc.createElement("Haupttrunk");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Haupttrunk Bemerkung
					komentar = doc.createComment("Bemerkung zum Haupttrunk");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("HaupttrunkBemerkung");
					element = doc.createElement("HaupttrunkBemerkung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Nachtrunk
					komentar = doc.createComment("Verkostungseinstellung Nachtrunk binaer codiert");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Nachtrunk");
					element = doc.createElement("Nachtrunk");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Nachtrunk Bemerkung
					komentar = doc.createComment("Bemerkung zum Nachtrunk");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("NachtrunkBemerkung");
					element = doc.createElement("NachtrunkBemerkung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Gesamteindruck
					komentar = doc.createComment("Verkostungseinstellung Gesamteindruck binaer codiert");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Gesamteindruck");
					element = doc.createElement("Gesamteindruck");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Gesamteindruck Bemerkung
					komentar = doc.createComment("Bemerkung zum Gesamteindruck");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("GesamteindruckBemerkung");
					element = doc.createElement("GesamteindruckBemerkung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);


					i++;
				}
			}
			daten.appendChild(sud);

			//Im Rezept verwendete Rohstoffe abspeichern
			QStringList sl;
			komentar = doc.createComment("Hier sind die Rohstoffe aufgelistet die Im Sud verwendet wurden");
			daten.appendChild(komentar);
			QDomElement Rohstoffe = doc.createElement("Rohstoffe");
			daten.appendChild(Rohstoffe);
			
			//Malz
			sl.clear();
			sql = "SELECT * FROM Malzschuettung WHERE SudID=" + QString::number(SudNr) + ";";
			if (!query_Malz.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rueckgabe:\n") + query_Malz.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				QDomElement Malz = doc.createElement("Malz");
				Rohstoffe.appendChild(Malz);
				QDomElement Eintrag;

				int i=0;
				while (query_Malz.next()){
					FeldNr = query_Malz.record().indexOf("Name");
					sql = "SELECT * FROM Malz WHERE Beschreibung='" + query_Malz.value(FeldNr).toString().replace("'","''") + "';";
					if (!query.exec(sql)) {
						// Fehlermeldung Datenbankabfrage
						ErrorMessage *errorMessage = new ErrorMessage();
						errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
							CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
							+ trUtf8("\nSQL Befehl:\n") + sql);
					}
					else {
						//Wenn Eintrag in Schüttungsauswahl vorhanden ist Eintrag Schreiben
						if (query.first()){
							//Überprüfen ob Eintrag schon vorher geschrieben wurde
							FeldNr = query.record().indexOf("Beschreibung");
							QString name = query.value(FeldNr).toString();
							if (!sl.contains(name)){
								sl.append(name);
								Eintrag = doc.createElement("Eintrag_" + QString::number(i + 1));
								Malz.appendChild(Eintrag);
			
								//Beschreibung
								komentar = doc.createComment("Malzbeschreibung (Name)");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Beschreibung");
								element = doc.createElement("Beschreibung");
								element.setAttribute("Einheit","Text");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);
			
								//Farbe
								komentar = doc.createComment("Malzfarbwert");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Farbe");
								element = doc.createElement("Farbe");
								element.setAttribute("Einheit","EBU");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);
			
								//Maximaler Schüttungsanteil
								komentar = doc.createComment("Maximal empfohlener Schuettungsanteil");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("MaxProzent");
								element = doc.createElement("MaxProzent");
								element.setAttribute("Einheit","Prozent");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);
			
								//Menge
								komentar = doc.createComment("Vorhandene Rohstoffmenge");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Menge");
								element = doc.createElement("Menge");
								element.setAttribute("Einheit","Kg");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);
			
								//Preis
								komentar = doc.createComment("Einkaufspreis");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Preis");
								element = doc.createElement("Preis");
								element.setAttribute("Einheit","Euro/Kg");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);
			
								//Bemerkung
								komentar = doc.createComment("Bemerkung");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Bemerkung");
								element = doc.createElement("Bemerkung");
								element.setAttribute("Einheit","Text");
								s = query.value(FeldNr).toString();
								//Alle Zeilenumbrueche gegen <br> austauschen
								s.replace('\n',"<br>");
								text = doc.createCDATASection(s);
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Anwendung
								komentar = doc.createComment("Anwendung");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Anwendung");
								element = doc.createElement("Anwendung");
								element.setAttribute("Einheit","Text");
								s = query.value(FeldNr).toString();
								//Alle Zeilenumbrueche gegen <br> austauschen
								s.replace('\n',"<br>");
								text = doc.createCDATASection(s);
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Datum Eingelagert
								komentar = doc.createComment("Datum Eingelagert");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Eingelagert");
								element = doc.createElement("Eingelagert");
								element.setAttribute("Einheit","Qt::ISODate");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Datum Mindesthaltbar
								komentar = doc.createComment("Datum Mindesthaltbar");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Mindesthaltbar");
								element = doc.createElement("Mindesthaltbar");
								element.setAttribute("Einheit","Qt::ISODate");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								i++;
							}
						}
					}
				}
			}

			//Hopfen Im Rezept verwendet
			sl.clear();
			sql = "SELECT * FROM Hopfengaben WHERE Aktiv='1' AND SudID=" + QString::number(SudNr) + ";";
			QDomElement Hopfen = doc.createElement("Hopfen");
			int i=0;
			if (!query_Hopfen.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rueckgabe:\n") + query_Malz.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				Rohstoffe.appendChild(Hopfen);
				QDomElement Eintrag;

				while (query_Hopfen.next()){
					FeldNr = query_Hopfen.record().indexOf("Name");
					sql = "SELECT * FROM Hopfen WHERE Beschreibung='" + query_Hopfen.value(FeldNr).toString().replace("'","''") + "';";
					if (!query.exec(sql)) {
						// Fehlermeldung Datenbankabfrage
						ErrorMessage *errorMessage = new ErrorMessage();
						errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
							CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
							+ trUtf8("\nSQL Befehl:\n") + sql);
					}
					else {
						//Wenn Eintrag in Schüttungsauswahl vorhanden ist Eintrag Schreiben
						if (query.first()){
							//Überprüfen ob Eintrag schon vorher geschrieben wurde
							FeldNr = query.record().indexOf("Beschreibung");
							QString name = query.value(FeldNr).toString();
							if (!sl.contains(name)){
								sl.append(name);
								Eintrag = doc.createElement("Eintrag_" + QString::number(i + 1));
								Hopfen.appendChild(Eintrag);
			
								//Beschreibung
								komentar = doc.createComment("Hopfenbeschreibung (Name)");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Beschreibung");
								element = doc.createElement("Beschreibung");
								element.setAttribute("Einheit","Text");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);
	
								//Alpha
								komentar = doc.createComment("Alphaprozentgehalt");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Alpha");
								element = doc.createElement("Alpha");
								element.setAttribute("Einheit","Alpha Prozent");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);
	
								//Menge
								komentar = doc.createComment("Vorhandene Menge");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Menge");
								element = doc.createElement("Menge");
								element.setAttribute("Einheit","Gramm");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);
	
								//Preis
								komentar = doc.createComment("Einkaufspreis");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Preis");
								element = doc.createElement("Preis");
								element.setAttribute("Einheit","Euro/Kg");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);
	
								//Pellets
								komentar = doc.createComment("Hopfenart 1=Pellets");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Pellets");
								element = doc.createElement("Pellets");
								element.setAttribute("Einheit","Text");
								text = doc.createTextNode(QString::number(query.value(FeldNr).toBool()));
								element.appendChild(text);
								Eintrag.appendChild(element);
	
								//Bemerkung
								komentar = doc.createComment("Bemerkung");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Bemerkung");
								element = doc.createElement("Bemerkung");
								element.setAttribute("Einheit","Text");
								s = query.value(FeldNr).toString();
								//Alle Zeilenumbrueche gegen <br> austauschen
								s.replace('\n',"<br>");
								text = doc.createCDATASection(s);
								element.appendChild(text);
								Eintrag.appendChild(element);
	
								//Eigenschaften
								komentar = doc.createComment("Eigenschaften");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Eigenschaften");
								element = doc.createElement("Eigenschaften");
								element.setAttribute("Einheit","Text");
								s = query.value(FeldNr).toString();
								//Alle Zeilenumbrueche gegen <br> austauschen
								s.replace('\n',"<br>");
								text = doc.createCDATASection(s);
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Typ
								komentar = doc.createComment("Typ 1=Aroma 2=Bitter 3=Universal");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Typ");
								element = doc.createElement("Typ");
								element.setAttribute("Einheit","Integer");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Datum Eingelagert
								komentar = doc.createComment("Datum Eingelagert");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Eingelagert");
								element = doc.createElement("Eingelagert");
								element.setAttribute("Einheit","Qt::ISODate");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Datum Mindesthaltbar
								komentar = doc.createComment("Datum Mindesthaltbar");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Mindesthaltbar");
								element = doc.createElement("Mindesthaltbar");
								element.setAttribute("Einheit","Qt::ISODate");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								i++;
							}
						}
					}
				}
			}
			
			//Hopfen In Weiteren Zutaten
			sl.clear();
			sql = "SELECT * FROM WeitereZutatenGaben WHERE Typ='100' AND SudID=" + QString::number(SudNr) + ";";
			if (!query_Hopfen.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rueckgabe:\n") + query_Hopfen.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				//QDomElement Hopfen = doc.createElement("Hopfen");
				//Rohstoffe.appendChild(Hopfen);
				QDomElement Eintrag;

				while (query_Hopfen.next()){
					FeldNr = query_Hopfen.record().indexOf("Name");
					sql = "SELECT * FROM Hopfen WHERE Beschreibung='" + query_Hopfen.value(FeldNr).toString().replace("'","''") + "';";
					if (!query.exec(sql)) {
						// Fehlermeldung Datenbankabfrage
						ErrorMessage *errorMessage = new ErrorMessage();
						errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
							CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
							+ trUtf8("\nSQL Befehl:\n") + sql);
					}
					else {
						//Wenn Eintrag in Schüttungsauswahl vorhanden ist Eintrag Schreiben
						if (query.first()){
							//Überprüfen ob Eintrag schon vorher geschrieben wurde
							FeldNr = query.record().indexOf("Beschreibung");
							QString name = query.value(FeldNr).toString();
							if (!sl.contains(name)){
								sl.append(name);
								Eintrag = doc.createElement("Eintrag_" + QString::number(i + 1));
								Hopfen.appendChild(Eintrag);

								//Beschreibung
								komentar = doc.createComment("Hopfenbeschreibung (Name)");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Beschreibung");
								element = doc.createElement("Beschreibung");
								element.setAttribute("Einheit","Text");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Alpha
								komentar = doc.createComment("Alphaprozentgehalt");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Alpha");
								element = doc.createElement("Alpha");
								element.setAttribute("Einheit","Alpha Prozent");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Menge
								komentar = doc.createComment("Vorhandene Menge");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Menge");
								element = doc.createElement("Menge");
								element.setAttribute("Einheit","Gramm");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Preis
								komentar = doc.createComment("Einkaufspreis");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Preis");
								element = doc.createElement("Preis");
								element.setAttribute("Einheit","Euro/Kg");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Pellets
								komentar = doc.createComment("Hopfenart 1=Pellets");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Pellets");
								element = doc.createElement("Pellets");
								element.setAttribute("Einheit","Text");
								text = doc.createTextNode(QString::number(query.value(FeldNr).toBool()));
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Bemerkung
								komentar = doc.createComment("Bemerkung");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Bemerkung");
								element = doc.createElement("Bemerkung");
								element.setAttribute("Einheit","Text");
								s = query.value(FeldNr).toString();
								//Alle Zeilenumbrueche gegen <br> austauschen
								s.replace('\n',"<br>");
								text = doc.createCDATASection(s);
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Eigenschaften
								komentar = doc.createComment("Eigenschaften");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Eigenschaften");
								element = doc.createElement("Eigenschaften");
								element.setAttribute("Einheit","Text");
								s = query.value(FeldNr).toString();
								//Alle Zeilenumbrueche gegen <br> austauschen
								s.replace('\n',"<br>");
								text = doc.createCDATASection(s);
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Typ
								komentar = doc.createComment("Typ 1=Aroma 2=Bitter 3=Universal");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Typ");
								element = doc.createElement("Typ");
								element.setAttribute("Einheit","Integer");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Datum Eingelagert
								komentar = doc.createComment("Datum Eingelagert");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Eingelagert");
								element = doc.createElement("Eingelagert");
								element.setAttribute("Einheit","Qt::ISODate");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Datum Mindesthaltbar
								komentar = doc.createComment("Datum Mindesthaltbar");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Mindesthaltbar");
								element = doc.createElement("Mindesthaltbar");
								element.setAttribute("Einheit","Qt::ISODate");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								i++;
							}
						}
					}
				}
			}

			//Verwendete Hefe
			QDomElement Hefe = doc.createElement("Hefe");
			Rohstoffe.appendChild(Hefe);
			QDomElement Eintrag;
			sql = "SELECT * FROM Hefe WHERE Beschreibung='" + AuswahlHefe.replace("'","''") + "';";
			if (!query.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				if (query.first()){
					Eintrag = doc.createElement("Eintrag_" + QString::number(1));
					Hefe.appendChild(Eintrag);
					//Beschreibung
					komentar = doc.createComment("Hefebeschreibung (Name)");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Beschreibung");
					element = doc.createElement("Beschreibung");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Menge
					komentar = doc.createComment("Vorhandene Menge");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Menge");
					element = doc.createElement("Menge");
					element.setAttribute("Einheit","Einheiten");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//BenoetigteMenge
					komentar = doc.createComment("Benoetigte Einheiten");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Einheiten");
					element = doc.createElement("Einheiten");
					element.setAttribute("Einheit","Einheiten");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Preis
					komentar = doc.createComment("Einkaufspreis");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Preis");
					element = doc.createElement("Preis");
					element.setAttribute("Einheit","Euro/Einheit");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Bemerkung
					komentar = doc.createComment("Bemerkung");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Bemerkung");
					element = doc.createElement("Bemerkung");
					element.setAttribute("Einheit","Text");
					s = query.value(FeldNr).toString();
					//Alle Zeilenumbrueche gegen <br> austauschen
					s.replace('\n',"<br>");
					text = doc.createCDATASection(s);
					element.appendChild(text);
					Eintrag.appendChild(element);

					//TypOGUG
					komentar = doc.createComment("TypOGUG 1=Obergaerig 2=Untergaerig");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("TypOGUG");
					element = doc.createElement("TypOGUG");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//TypTrFl
					komentar = doc.createComment("TypTrFl 1=Trocken 2=Fluessig");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("TypTrFl");
					element = doc.createElement("TypTrFl");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Verpackungsmenge
					komentar = doc.createComment("Verpackungsmenge");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Verpackungsmenge");
					element = doc.createElement("Verpackungsmenge");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Wuerzemenge
					komentar = doc.createComment("Wuerzemenge");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Wuerzemenge");
					element = doc.createElement("Wuerzemenge");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Eigenschaften
					komentar = doc.createComment("Eigenschaften");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Eigenschaften");
					element = doc.createElement("Eigenschaften");
					element.setAttribute("Einheit","Text");
					s = query.value(FeldNr).toString();
					//Alle Zeilenumbrueche gegen <br> austauschen
					s.replace('\n',"<br>");
					text = doc.createCDATASection(s);
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Sedimentation
					komentar = doc.createComment("SED 1=Hoch 2=Mittel 3=Niedrig");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("SED");
					element = doc.createElement("SED");
					element.setAttribute("Einheit","Integer");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//EVG
					komentar = doc.createComment("EVG Endvergaerungsgrad");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("EVG");
					element = doc.createElement("EVG");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Temperatur
					komentar = doc.createComment("Temperatur");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Temperatur");
					element = doc.createElement("Temperatur");
					element.setAttribute("Einheit","Text");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Datum Eingelagert
					komentar = doc.createComment("Datum Eingelagert");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Eingelagert");
					element = doc.createElement("Eingelagert");
					element.setAttribute("Einheit","Qt::ISODate");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);

					//Datum Mindesthaltbar
					komentar = doc.createComment("Datum Mindesthaltbar");
					Eintrag.appendChild(komentar);
					FeldNr = query.record().indexOf("Mindesthaltbar");
					element = doc.createElement("Mindesthaltbar");
					element.setAttribute("Einheit","Qt::ISODate");
					text = doc.createTextNode(query.value(FeldNr).toString());
					element.appendChild(text);
					Eintrag.appendChild(element);
				}
			}

			//Weitere Zutaten
			sl.clear();
			sql = "SELECT * FROM WeitereZutatenGaben WHERE Typ!='100' AND SudID=" + QString::number(SudNr) + ";";
			QSqlQuery WeitereZutatenGaben;
			if (!WeitereZutatenGaben.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rueckgabe:\n") + WeitereZutatenGaben.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				QDomElement WeitereZutaten = doc.createElement("WeitereZutaten");
				Rohstoffe.appendChild(WeitereZutaten);
				QDomElement Eintrag;

				int i=0;
				while (WeitereZutatenGaben.next()){
					FeldNr = WeitereZutatenGaben.record().indexOf("Name");
					sql = "SELECT * FROM WeitereZutaten WHERE Beschreibung='" + WeitereZutatenGaben.value(FeldNr).toString().replace("'","''") + "';";
					if (!query.exec(sql)) {
						// Fehlermeldung Datenbankabfrage
						ErrorMessage *errorMessage = new ErrorMessage();
						errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
							CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
							+ trUtf8("\nSQL Befehl:\n") + sql);
					}
					else {
						//Wenn Eintrag in Schüttungsauswahl vorhanden ist Eintrag Schreiben
						if (query.first()){
							//Überprüfen ob Eintrag schon vorher geschrieben wurde
							FeldNr = query.record().indexOf("Beschreibung");
							QString name = query.value(FeldNr).toString();
							if (!sl.contains(name)){
								sl.append(name);
								Eintrag = doc.createElement("Eintrag_" + QString::number(i + 1));
								WeitereZutaten.appendChild(Eintrag);

								//Beschreibung
								komentar = doc.createComment("Name der Zutat");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Beschreibung");
								element = doc.createElement("Beschreibung");
								element.setAttribute("Einheit","Text");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Menge
								komentar = doc.createComment("Vorhandene Menge");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Menge");
								element = doc.createElement("Menge");
								element.setAttribute("Einheit","Kg/g");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Einheiten
								komentar = doc.createComment("Einheiten Typ 0=Kg 1=g");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Einheiten");
								element = doc.createElement("Einheiten");
								element.setAttribute("Einheit","Integer");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Typ
								komentar = doc.createComment("Art der Zutat 0=Honig 1=Zucker 2=Gewuerz 3=Frucht 4=Sonstiges 100=Hopfen");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Typ");
								element = doc.createElement("Typ");
								element.setAttribute("Einheit","Integer");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Ausbeute
								komentar = doc.createComment("Ausbeute 0= Keine Auswirkung auf die Stammwuerze");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Ausbeute");
								element = doc.createElement("Ausbeute");
								element.setAttribute("Einheit","Prozent");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Farbe
								komentar = doc.createComment("Farbe 0=Keine Auswirkung auf die Farbe");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("EBC");
								element = doc.createElement("EBC");
								element.setAttribute("Einheit","Prozent");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Preis
								komentar = doc.createComment("Einkaufspreis");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Preis");
								element = doc.createElement("Preis");
								element.setAttribute("Einheit","Euro/Kg");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Bemerkung
								komentar = doc.createComment("Bemerkung");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Bemerkung");
								element = doc.createElement("Bemerkung");
								element.setAttribute("Einheit","Text");
								s = query.value(FeldNr).toString();
								//Alle Zeilenumbrueche gegen <br> austauschen
								s.replace('\n',"<br>");
								text = doc.createCDATASection(s);
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Datum Eingelagert
								komentar = doc.createComment("Datum Eingelagert");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Eingelagert");
								element = doc.createElement("Eingelagert");
								element.setAttribute("Einheit","Qt::ISODate");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								//Datum Mindesthaltbar
								komentar = doc.createComment("Datum Mindesthaltbar");
								Eintrag.appendChild(komentar);
								FeldNr = query.record().indexOf("Mindesthaltbar");
								element = doc.createElement("Mindesthaltbar");
								element.setAttribute("Einheit","Qt::ISODate");
								text = doc.createTextNode(query.value(FeldNr).toString());
								element.appendChild(text);
								Eintrag.appendChild(element);

								i++;
							}
						}
					}
				}
			}

			//xml Datei Speichern
			QFile file(Dateiname);
			if (!file.open(QFile::WriteOnly | QFile::Text)) {
				//Kann Suddatei nicht erstellen
				return 1;
			}
			
			QTextStream out(&file);
			const int IndentSize = 2;
			doc.save(out, IndentSize);
					
		}
	}
	return 0;
}


int QExport::IfXmlOK(QString cDateiname)
{
	QDomDocument doc("mydoc");
	QFile file(cDateiname);
	int rueckgabe = 0;
	if (!file.open(QIODevice::ReadOnly))
		return 1; //Datei kann nicht geöffnet werden
	if (!doc.setContent(&file, &errMsg, &errLine, &errCol)) {
		file.close(); //Fehler beim parsen der xml Datei
		return 2;
	}
	file.close();
	QDomElement root = doc.documentElement();
	if (root.tagName() != "xsud") {
		rueckgabe = 3;
	}
	else {
		//Versionsstände kontrolieren
		QDomElement version = root.firstChildElement("Version");
		if (!version.isNull()) {
			//Version der Datenbank auslesen
			QDomElement e = version.firstChildElement("Datenbank");
			if (!e.isNull()) {
				VersionDatenbank = e.text().toInt();
			}
			else {
				rueckgabe = 3;
			}
			//Version der xml Datei auslesen
			e = version.firstChildElement("xsud");
			if (!e.isNull()) {
				VersionXML = e.text().toInt();
			}
			else {
				rueckgabe = 3;
			}
		}
		else {
			rueckgabe = 3;
		}
	}
	
	//Error Msg Text setzten
	if (rueckgabe == 3) {
		errMsg = trUtf8("Die Datei ist keine gültige xsud Importdatei");
		return 3;
	}
	return 0;
}


int QExport::ImportSudXML(QString cDateiname)
{
	QDomDocument doc("mydoc");
	Dateiname = cDateiname;
	QFile file(cDateiname);
	int rueckgabe = 0;
	if (!file.open(QIODevice::ReadOnly))
		return 1; //Datei kann nicht geöffnet werden
	if (!doc.setContent(&file, &errMsg, &errLine, &errCol)) {
		file.close(); //Fehler beim parsen der xml Datei
		return 2;
	}
	file.close();
	QDomElement root = doc.documentElement();
	if (root.tagName() != "xsud") {
		errMsg = trUtf8("Die Datei ist keine gültige xsud Importdatei");
		rueckgabe = 3;
	}
	else {
		//Suddaten auslesen
		QDomElement Sud = root.firstChildElement("Sud");
		if (!Sud.isNull()) {
			QSqlDatabase::database().transaction();
			QSqlQuery query;
			QString s;
			QString sql = "INSERT INTO Sud (";
			sql += "'Sudname',";
			sql += "'Erstellt' ,";
			sql += "'Gespeichert' ,";
			sql += "'Menge',";
			sql += "'SW',";
			sql += "'CO2',";
			sql += "'IBU',";
			sql += "'Reifezeit' ,";
			sql += "'Kommentar' ,";
			sql += "'Braudatum',";
			sql += "'BierWurdeGebraut' ,";
			sql += "'BierWurdeAbgefuellt' ,";
			sql += "'BierWurdeVerbraucht' ,";
			sql += "'KochdauerNachBitterhopfung' ,";
			sql += "'FaktorHauptguss' ,";
			sql += "'AuswahlHefe' ,";
			sql += "'HefeAnzahlEinheiten' ,";
			sql += "'WuerzemengeVorHopfenseihen' ,";
			sql += "'SWVorHopfenseihen' ,";
			sql += "'WuerzemengeKochende' ,";
			sql += "'SWKochende'  ,";
			sql += "'Speisemenge' ,";
			sql += "'Anstelldatum' ,";
			sql += "'WuerzemengeAnstellen'  ,";
			sql += "'SWAnstellen'  ,";
			sql += "'Abfuelldatum',";
			sql += "'SWSchnellgaerprobe' ,";
			sql += "'SWJungbier' ,";
			sql += "'TemperaturJungbier' ,";
			sql += "'EinmaischenTemp' ,";
			sql += "'KostenWasserStrom' ,";
			sql += "'Nachisomerisierungszeit' ,";
			sql += "'AktivTab' ,";
			sql += "'AktivTab_Gaerverlauf' ,";
			sql += "'RestalkalitaetSoll' ,";
			sql += "'SchnellgaerprobeAktiv' ,";
			sql += "'JungbiermengeAbfuellen' ,";
			sql += "'Bewertung' ,";
			sql += "'BewertungText' ,";
			sql += "'BewertungMaxSterne' ,";
			sql += "'berechnungsArtHopfen' ,";
			sql += "'highGravityFaktor' ,";
			sql += "'erg_S_Gesammt'  ,";
			sql += "'erg_W_Gesammt' ,";
			sql += "'erg_WHauptguss'  ,";
			sql += "'erg_WNachguss'  ,";
			sql += "'erg_Sudhausausbeute' ,";
			sql += "'erg_Farbe'  ,";
			sql += "'erg_Preis' ,";
			sql += "'erg_Alkohol', ";
			sql += "'erg_EffektiveAusbeute', ";
			sql += "'erg_AbgefuellteBiermenge') ";
			sql += "Values(";
			//Sudname
			QDomElement e = Sud.firstChildElement("Sudname");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'Import " + s + "',";
			//Zeitstempel fuer Erstellungsdatum in der Datenbank
			e = Sud.firstChildElement("Erstellt");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Zeitstempel wann Sud das letzte-mal in der Datenbank gespeichert wurde
			e = Sud.firstChildElement("Gespeichert");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Soll Wuerze-Anstellmenge
			e = Sud.firstChildElement("Menge");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Soll Stammwuerze
			e = Sud.firstChildElement("SW");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Soll CO2 Gehalt
			e = Sud.firstChildElement("CO2");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Soll Bittere
			e = Sud.firstChildElement("IBU");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Angepeilte Reifezeit
			e = Sud.firstChildElement("Reifezeit");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Allgemeiner Kommentar
			e = Sud.firstChildElement("Kommentar");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Datum an dem der Sud gebraut wurde
			e = Sud.firstChildElement("Braudatum");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Flag ob Sud gebraut wurde
			e = Sud.firstChildElement("BierWurdeGebraut");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Flag ob Sud abgefuellt wurde
			bool Abgefuellt = false;
			e = Sud.firstChildElement("BierWurdeAbgefuellt");
			if (!e.isNull()){
				s = e.text();
				if (e.text() == "1")
					Abgefuellt = true;
			}
			else
				s = "";
			sql += "'" + s + "',";
			//Flag ob Sud verbraucht wurde
			e = Sud.firstChildElement("BierWurdeVerbraucht");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Kochdauer nach der ersten Hopfengabe
			e = Sud.firstChildElement("KochdauerNachBitterhopfung");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Faktor zum Berechnen der Hauptgussmenge (Schuettung * Faktor = Hauptgussmenge)
			e = Sud.firstChildElement("FaktorHauptguss");
			if (!e.isNull())
				s = e.text().replace("'","''");
			else
				s = "";
			sql += "'" + s + "',";
			//Name der Ausgewaehlten Hefe
			e = Sud.firstChildElement("AuswahlHefe");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			if (s != ""){
				AustauschRohstoff = "";
				CheckHefeEintrag(s);
				if (AustauschRohstoff != ""){
					s = AustauschRohstoff;
				}
			}
			sql += "'" + s.replace("'","''") + "',";
			//Anzahl verwendeter Hefe Einheiten
			e = Sud.firstChildElement("HefeAnzahlEinheiten");
			if (!e.isNull())
				s = e.text();
			else
				s = "1";
			sql += "'" + s + "',";
			//Wuerzemenge vor dem Hopfenseihen
			e = Sud.firstChildElement("WuerzemengeVorHopfenseihen");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Stammwuerze vor dem Hopfenseihen
			e = Sud.firstChildElement("SWVorHopfenseihen");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Wuerzemenge nach dem Hopfenseihen
			e = Sud.firstChildElement("WuerzemengeKochende");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Stammwuerze nach dem Hopfenseihen
			e = Sud.firstChildElement("SWKochende");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Abgefuellte Speisemenge
			double AbgefuellteSpeisemenge = 0;
			e = Sud.firstChildElement("Speisemenge");
			if (!e.isNull()){
				s = e.text();
				AbgefuellteSpeisemenge = e.text().toDouble();
			}
			else
				s = "";
			sql += "'" + s + "',";
			//Datum der Hefezugabe
			e = Sud.firstChildElement("Anstelldatum");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Wuerzemenge beim der Hefezugabe
			e = Sud.firstChildElement("WuerzemengeAnstellen");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Stammwuerze bei der Hefezugabe
			e = Sud.firstChildElement("SWAnstellen");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Abfuelldatum
			e = Sud.firstChildElement("Abfuelldatum");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Restextrakt der Schnellgaerprobe
			e = Sud.firstChildElement("SWSchnellgaerprobe");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Restextrakt des Jungbieres aus der Hauptgaerung
			e = Sud.firstChildElement("SWJungbier");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Temperatur Jungbier beim Abfuellen
			e = Sud.firstChildElement("TemperaturJungbier");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Temperatur Einmaischen
			e = Sud.firstChildElement("EinmaischenTemp");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Kosten fuer Wasser / Strom / Gas etc.
			e = Sud.firstChildElement("KostenWasserStrom");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Dauer die der Hopfen noch nach dem Kochen Bittere abgeben kann
			e = Sud.firstChildElement("Nachisomerisierungszeit");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Aktives Tab im kleinen-brauhelfer nach dem Laden
			e = Sud.firstChildElement("AktivTab");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Aktives Tab im Gaerverlauf nach dem Laden
			e = Sud.firstChildElement("AktivTab_Gaerverlauf");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Gewünschte Restalkalität
			e = Sud.firstChildElement("RestalkalitaetSoll");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Flag ob eine Schnellgärprobe gemacht wurde
			e = Sud.firstChildElement("SchnellgaerprobeAktiv");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Jungbiermenge beim Abfüllen
			double Jungbiermenge = 0;
			e = Sud.firstChildElement("JungbiermengeAbfuellen");
			if (!e.isNull()){
				s = e.text();
				Jungbiermenge = e.text().toDouble();
			}
			//Wenn Eintrag nicht vorhanden ist Würzemenge beim Anstellen verwenden
			else {
				e = Sud.firstChildElement("WuerzemengeAnstellen");
				if (!e.isNull()){
					s = e.text();
					Jungbiermenge = e.text().toDouble();
				}
				else
					s = "";
			}
			sql += "'" + s + "',";
			//Beste Bewertung (Anzahl Sterne)
			e = Sud.firstChildElement("Bewertung");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Reifewoche der Besten Bewertung
			e = Sud.firstChildElement("BewertungText");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Maximale Anzahl Sterne bei diesem Sud
			e = Sud.firstChildElement("BewertungMaxSterne");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Art der Hopfenberechnung
			e = Sud.firstChildElement("berechnungsArtHopfen");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//High Gravity Faktor
			e = Sud.firstChildElement("highGravityFaktor");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Gesammtmenge Schuettung
			e = Sud.firstChildElement("erg_S_Gesammt");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Gesammtmenge Wasser
			e = Sud.firstChildElement("erg_W_Gesammt");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Menge Hauptguss
			e = Sud.firstChildElement("erg_WHauptguss");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Menge Nachguss
			e = Sud.firstChildElement("erg_WNachguss");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Sudhausausbeute
			e = Sud.firstChildElement("erg_Sudhausausbeute");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Bierfarbe
			e = Sud.firstChildElement("erg_Farbe");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Kosten pro Liter Bier
			e = Sud.firstChildElement("erg_Preis");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Alkoholgehalt
			e = Sud.firstChildElement("erg_Alkohol");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Effektive Ausbeute von diesem Sud
			e = Sud.firstChildElement("erg_EffektiveAusbeute");
			if (!e.isNull())
				s = e.text();
			else
				s = "";
			sql += "'" + s + "',";
			//Abgefüllte Biermenge
			e = Sud.firstChildElement("erg_AbgefuellteBiermenge");
			if (!e.isNull())
				s = e.text();
			else {
				s = "";
				//Wenn der Eintrag nicht vorhanden ist und Bier schon abgefüllt wurde
				if (Abgefuellt){
					//Wenn Abgefüllte Speisemenge nicht 0 ist einen Hinweis ausgeben das die Menge neu berechnet werden muss.
					if (AbgefuellteSpeisemenge > 0){
						HinweisAusgeben(trUtf8("In dieser Sud Datei ist der Eintrag Abgefüllte Biermenge noch nicht vorhanden\nDas die Menge in der Brauübersicht richtig dargestellt wird muss dieser Sud einmal geladen und weider gespeichert werden, dann wird dieser Wert neu berechnet."));
					}
					else {
						s = QString::number(Jungbiermenge);
					}
				}
			}
			sql += "'" + s + "'";
			sql += ");";

			//Abfrage Abschicken
			if (!query.exec(sql)) {
				// Fehlermeldung Datenbankabfrage
				ErrorMessage *errorMessage = new ErrorMessage();
				errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
					CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
					+ trUtf8("\nSQL Befehl:\n") + sql);
			}
			else {
				//SudID auslesen
				sql = "SELECT last_insert_rowid();";
				if (!query.exec(sql)) {
					// Fehlermeldung Datenbankabfrage
					ErrorMessage *errorMessage = new ErrorMessage();
					errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
						CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
						+ trUtf8("\nSQL Befehl:\n") + sql);
				}
				else {
					query.first();
					QString SudIDNeu = query.value(0).toString();
					//Rasten eintragen
					QDomElement Rasten = Sud.firstChildElement("Rasten");
					if (!Rasten.isNull()) {
						QDomNode n = Rasten.firstChild();
						while(!n.isNull()) {
							QDomNode rast = n;//.toElement(); // try to convert the node to an element.
							if(!rast.isNull()) {
								//Datensatz erstellen
								sql = "INSERT INTO Rasten ";
								sql += "(";
								sql += "'SudID', ";
								sql += "'RastAktiv', ";
								sql += "'RastTemp', ";
								sql += "'RastDauer', ";
								sql += "'RastName' ";
								sql += ")Values(";
								sql += "'" + SudIDNeu + "',";
								//Rast Aktiv
								e = rast.firstChildElement("RastAktiv");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Rast Temperatur
								e = rast.firstChildElement("RastTemp");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Rast Dauer
								e = rast.firstChildElement("RastDauer");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Rast Name
								e = rast.firstChildElement("RastName");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "'";
								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
							n = n.nextSibling();
						}
					}
					
					//Schüttung eintragen
					QDomElement Schuettung = Sud.firstChildElement("Schuettung");
					if (!Schuettung.isNull()) {
						QDomNode n = Schuettung.firstChild();
						while(!n.isNull()) {
							QDomNode Anteil = n;
							if(!Anteil.isNull()) {
								//Datensatz erstellen
								sql = "INSERT INTO Malzschuettung ";
								sql += "(";
								sql += "'SudID', ";
								sql += "'Name', ";
								sql += "'Prozent', ";
								sql += "'erg_Menge', ";
								sql += "'Farbe' ";
								sql += ")Values(";
								sql += "'" + SudIDNeu + "',";
								//Malzbeschreibung (Name)
								e = Anteil.firstChildElement("Name");
								if (!e.isNull()){
									s = e.text();
									//Überprüfen ob Malzeintrag schon vorhanden ist und gegebenenfalls
									//in die Datenbank eintragen.
									if (s != ""){
										AustauschRohstoff = "";
										CheckMalzEintrag(s);
										if (AustauschRohstoff != ""){
											s = AustauschRohstoff;
										}
									}
								}
								else
									s = "";
								sql += "'" + s.replace("'","''") + "',";
								//Prozentualer Anteil der Schuettung
								e = Anteil.firstChildElement("Prozent");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Berechneter Gewichtsanteil der Schuettung
								e = Anteil.firstChildElement("erg_Menge");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Farbe
								e = Anteil.firstChildElement("Farbe");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "'";
								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
							n = n.nextSibling();
						}
					}
					
					//Hopfengaben eintragen
					QDomElement Hopfengaben = Sud.firstChildElement("Hopfengaben");
					if (!Hopfengaben.isNull()) {
						QDomNode n = Hopfengaben.firstChild();
						while(!n.isNull()) {
							QDomNode Anteil = n;
							if(!Anteil.isNull()) {
								//Datensatz erstellen
								sql = "INSERT INTO Hopfengaben ";
								sql += "(";
								sql += "'SudID', ";
								sql += "'Aktiv', ";
								sql += "'Name', ";
								sql += "'Prozent', ";
								sql += "'Zeit', ";
								sql += "'erg_Menge', ";
								sql += "'erg_Hopfentext', ";
								sql += "'Alpha', ";
								sql += "'Pellets', ";
								sql += "'Vorderwuerze' ";
								sql += ")Values(";
								sql += "'" + SudIDNeu + "',";
								//1 = Hopfengabe Aktiv
								e = Anteil.firstChildElement("Aktiv");
								bool b = false;
								if (!e.isNull()){
									s = e.text();
									if (s == "1")
										b = true;
								}
								else
									s = "";
								sql += "'" + s + "',";
								//Berschreibung Hopfen (Name)
								e = Anteil.firstChildElement("Name");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								if ( (s != "") && (b) ){
									AustauschRohstoff = "";
									CheckHopfenEintrag(s);
									if (AustauschRohstoff != ""){
										s = AustauschRohstoff;
									}
								}
								sql += "'" + s.replace("'","''") + "',";
								//Prozentualer Gewichts-anteil der Hopfengabe
								e = Anteil.firstChildElement("Prozent");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s.replace("'","''") + "',";
								//Zeit nach Hopfengabe 1
								e = Anteil.firstChildElement("Zeit");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Berechnete Gewichtsmenge
								e = Anteil.firstChildElement("erg_Menge");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Beschreibungstext
								e = Anteil.firstChildElement("erg_Hopfentext");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//Alphaprozent gehalt des Hopfens
								e = Anteil.firstChildElement("Alpha");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Hopfenart 1 = Pellets
								e = Anteil.firstChildElement("Pellets");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Vorderwürzehopfung
								e = Anteil.firstChildElement("Vorderwuerze");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "'";
								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
							n = n.nextSibling();
						}
					}
					
					//Weitere Zutaten Gaben eintragen
					QDomElement WeitereZutatenGaben = Sud.firstChildElement("WeitereZutatenGaben");
					if (!WeitereZutatenGaben.isNull()) {
						QDomNode n = WeitereZutatenGaben.firstChild();
						while(!n.isNull()) {
							QDomNode Anteil = n;
							if(!Anteil.isNull()) {
								//Datensatz erstellen
								sql = "INSERT INTO WeitereZutatenGaben ";
								sql += "(";
								sql += "'SudID', ";
								sql += "'Name', ";
								sql += "'Menge', ";
								sql += "'Einheit', ";
								sql += "'Typ', ";
								sql += "'Zeitpunkt', ";
								sql += "'Bemerkung', ";
								sql += "'erg_Menge', ";
								sql += "'Ausbeute', ";
								sql += "'Farbe' ";
								sql += ")Values(";
								sql += "'" + SudIDNeu + "',";
								//Name der Zutat (Name)
								e = Anteil.firstChildElement("Name");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								//Überprüfen ob Weitere Zutat schon vorhanden ist und gegebenenfalls
								//in die Datenbank eintragen.
								if (s != ""){
									e = Anteil.firstChildElement("Typ");
									if (!e.isNull()){
										//Wenn Typ Hopfen ist überprüfen ob Hopfen vorhanden ist
										if (e.text() == "100"){
											AustauschRohstoff = "";
											CheckHopfenEintrag(s);
											if (AustauschRohstoff != ""){
												s = AustauschRohstoff;
											}
										}
										//Wenn Typ Weitere Zutat ist bei den Weiteren Zutaten suchen
										else {
											AustauschRohstoff = "";
											CheckWeitereZutatEintrag(s);
											if (AustauschRohstoff != ""){
												s = AustauschRohstoff;
											}
										}
									}
									sql += "'" + s.replace("'","''") + "',";
								}
								//Menge
								e = Anteil.firstChildElement("Menge");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Einheiten Typ 0=Kg 1=g
								e = Anteil.firstChildElement("Einheit");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Art der Zutat 0=Honig 1=Zucker 2=Gewuerz 3=Frucht 4=Sonstiges 100=Hopfen
								e = Anteil.firstChildElement("Typ");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Zeitpunkt der Zugabe 0=Gaerung 1=Anstellen 2=Kochbeginn 3=Maischen
								e = Anteil.firstChildElement("Zeitpunkt");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Bemerkung
								e = Anteil.firstChildElement("Bemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//Berechnete Menge
								e = Anteil.firstChildElement("erg_Menge");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Ausbeute 0= Keine Auswirkung auf die Stammwürze
								e = Anteil.firstChildElement("Ausbeute");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Farbe 0=Keine Auswirkung auf die Farbe
								e = Anteil.firstChildElement("Farbe");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "'";
								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
							n = n.nextSibling();
						}
					}

					//Schnellgärverlauf einlesen
					QDomElement Schnellgaerverlauf = Sud.firstChildElement("Schnellgaerverlauf");
					if (!Schnellgaerverlauf.isNull()) {
						QDomNode n = Schnellgaerverlauf.firstChild();
						while(!n.isNull()) {
							QDomNode Eintrag = n;
							if(!Eintrag.isNull()) {
								//Datensatz erstellen
								sql = "INSERT INTO Schnellgaerverlauf ";
								sql += "(";
								sql += "'SudID', ";
								sql += "'Zeitstempel', ";
								sql += "'SW', ";
								sql += "'Alc', ";
								sql += "'Temp' ";
								sql += ")Values(";
								sql += "'" + SudIDNeu + "',";
								//Zeitstempel der Messung
								e = Eintrag.firstChildElement("Zeitstempel");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Restextrakt der Messung
								e = Eintrag.firstChildElement("SW");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Berechneter Alkoholgehalt der Messung
								e = Eintrag.firstChildElement("Alc");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Temperatur der Messung
								e = Eintrag.firstChildElement("Temp");
								if (!e.isNull())
									s = e.text();
								else
									s = "0";
								sql += "'" + s + "'";
								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
							n = n.nextSibling();
						}
					}
					
					
					//Hauptgärverlauf einlesen
					QDomElement Hauptgaerverlauf = Sud.firstChildElement("Hauptgaerverlauf");
					if (!Hauptgaerverlauf.isNull()) {
						QDomNode n = Hauptgaerverlauf.firstChild();
						while(!n.isNull()) {
							QDomNode Eintrag = n;
							if(!Eintrag.isNull()) {
								//Datensatz erstellen
								sql = "INSERT INTO Hauptgaerverlauf ";
								sql += "(";
								sql += "'SudID', ";
								sql += "'Zeitstempel', ";
								sql += "'SW', ";
								sql += "'Alc', ";
								sql += "'Temp' ";
								sql += ")Values(";
								sql += "'" + SudIDNeu + "',";
								//Zeitstempel der Messung
								e = Eintrag.firstChildElement("Zeitstempel");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Restextrakt der Messung
								e = Eintrag.firstChildElement("SW");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Berechneter Alkoholgehalt der Messung
								e = Eintrag.firstChildElement("Alc");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Temperatur der Messung
								e = Eintrag.firstChildElement("Temp");
								if (!e.isNull())
									s = e.text();
								else
									s = "0";
								sql += "'" + s + "'";
								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
							n = n.nextSibling();
						}
					}
					
					
					//Nachgärverlauf einlesen
					QDomElement Nachgaerverlauf = Sud.firstChildElement("Nachgaerverlauf");
					if (!Nachgaerverlauf.isNull()) {
						QDomNode n = Nachgaerverlauf.firstChild();
						while(!n.isNull()) {
							QDomNode Eintrag = n;
							if(!Eintrag.isNull()) {
								//Datensatz erstellen
								sql = "INSERT INTO Nachgaerverlauf ";
								sql += "(";
								sql += "'SudID', ";
								sql += "'Zeitstempel', ";
								sql += "'Druck', ";
								sql += "'Temp', ";
								sql += "'CO2' ";
								sql += ")Values(";
								sql += "'" + SudIDNeu + "',";
								//Zeitstempel der Messung
								e = Eintrag.firstChildElement("Zeitstempel");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Flaschen / Fassdruck der Messung
								e = Eintrag.firstChildElement("Druck");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Temperatur des Jungbieres
								e = Eintrag.firstChildElement("Temp");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Berechneter CO2 Gehalt des Jungbieres
								e = Eintrag.firstChildElement("CO2");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "'";
								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
							n = n.nextSibling();
						}
					}



					//Bewertungen einlesen
					QDomElement Bewertungen = Sud.firstChildElement("Bewertungen");
					if (!Bewertungen.isNull()) {
						QDomNode n = Bewertungen.firstChild();
						while(!n.isNull()) {
							QDomNode Eintrag = n;
							if(!Eintrag.isNull()) {
								//Datensatz erstellen
								sql = "INSERT INTO Bewertungen ";
								sql += "(";
								sql += "'SudID', ";
								sql += "'Woche', ";
								sql += "'Datum', ";
								sql += "'Sterne', ";
								sql += "'Bemerkung', ";
								sql += "'Farbe', ";
								sql += "'FarbeBemerkung', ";
								sql += "'Schaum', ";
								sql += "'SchaumBemerkung', ";
								sql += "'Geruch', ";
								sql += "'GeruchBemerkung', ";
								sql += "'Geschmack', ";
								sql += "'GeschmackBemerkung', ";
								sql += "'Antrunk', ";
								sql += "'AntrunkBemerkung', ";
								sql += "'Haupttrunk', ";
								sql += "'HaupttrunkBemerkung', ";
								sql += "'Nachtrunk', ";
								sql += "'NachtrunkBemerkung', ";
								sql += "'Gesamteindruck', ";
								sql += "'GesamteindruckBemerkung' ";
								sql += ")Values(";
								sql += "'" + SudIDNeu + "',";
								//
								e = Eintrag.firstChildElement("Woche");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Datum");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Sterne");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Bemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Farbe");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("FarbeBemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Schaum");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("SchaumBemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Geruch");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("GeruchBemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Geschmack");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("GeschmackBemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Antrunk");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("AntrunkBemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Haupttrunk");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("HaupttrunkBemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Nachtrunk");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("NachtrunkBemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("Gesamteindruck");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//
								e = Eintrag.firstChildElement("GesamteindruckBemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "'";
								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
							n = n.nextSibling();
						}
					}

				}
			}
			QSqlDatabase::database().commit();
		}
		else {
			errMsg = trUtf8("Die Datei ist keine gültige xsud Importdatei");
			rueckgabe = 3;
		}
	}
	return rueckgabe;
}

//Überprüft ob der Malzeintrag in der Datenbank vorhanden ist
void QExport::CheckMalzEintrag(QString Name)
{
	//
	QSqlQuery query;
	QString str = Name;
	QString sql = "SELECT ID FROM Malz WHERE BESCHREIBUNG='" + str.replace("'","''") + "';";
	if (!query.exec(sql)) {
		// Fehlermeldung Datenbankabfrage
		ErrorMessage *errorMessage = new ErrorMessage();
		errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
			CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
			+ trUtf8("\nSQL Befehl:\n") + sql);
	}
	if (query.first()){
		
	}
	else {
		//Abfrage ob fehlender Malzeintrag in die Rohstofftabelle eingetragen werden soll
		//Oder durch einen schon vorhandenen Ersetzt werden soll
		if (AbfrageRohstoffuebernahme(Name,"Malz")){
			//Eintrag aus xmlDatei auslesen und in Datenbank eintragen
			RohstoffMalzUebernehmen(Name);
		}
	}
}

void QExport::CheckHopfenEintrag(QString Name)
{
	//
	QSqlQuery query;
	QString str = Name;
	QString sql = "SELECT ID FROM Hopfen WHERE BESCHREIBUNG='" + str.replace("'","''") + "';";
	if (!query.exec(sql)) {
		// Fehlermeldung Datenbankabfrage
		ErrorMessage *errorMessage = new ErrorMessage();
		errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
			CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
			+ trUtf8("\nSQL Befehl:\n") + sql);
	}
	if (query.first()){

	}
	else {
		//Abfrage ob fehlender Malzeintrag in die Rohstofftabelle eingetragen werden soll
		if (AbfrageRohstoffuebernahme(Name,"Hopfen")){
			//Eintrag aus xmlDatei auslesen und in Datenbank eintragen
			RohstoffHopfenUebernehmen(Name);
		}
	}
}

void QExport::CheckHefeEintrag(QString Name)
{
	//
	QSqlQuery query;
	QString str = Name;
	QString sql = "SELECT ID FROM Hefe WHERE BESCHREIBUNG='" + str.replace("'","''") + "';";
	if (!query.exec(sql)) {
		// Fehlermeldung Datenbankabfrage
		ErrorMessage *errorMessage = new ErrorMessage();
		errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
			CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
			+ trUtf8("\nSQL Befehl:\n") + sql);
	}
	if (query.first()){

	}
	else {
		//Abfrage ob fehlender Malzeintrag in die Rohstofftabelle eingetragen werden soll
		if (AbfrageRohstoffuebernahme(Name,"Hefe")){
			//Eintrag aus xmlDatei auslesen und in Datenbank eintragen
			RohstoffHefeUebernehmen(Name);
		}
	}
}


void QExport::CheckWeitereZutatEintrag(QString Name)
{
	//
	QSqlQuery query;
	QString str = Name;
	QString sql = "SELECT ID FROM WeitereZutaten WHERE BESCHREIBUNG='" + str.replace("'","''") + "';";
	if (!query.exec(sql)) {
		// Fehlermeldung Datenbankabfrage
		ErrorMessage *errorMessage = new ErrorMessage();
		errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
			CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
			+ trUtf8("\nSQL Befehl:\n") + sql);
	}
	if (query.first()){

	}
	else {
		//Abfrage ob fehlender Eintrag in die Rohstofftabelle eingetragen werden soll
		if (AbfrageRohstoffuebernahme(Name,"WeitereZutaten")){
			//Eintrag aus xmlDatei auslesen und in Datenbank eintragen
			RohstoffWeitereZutatUebernehmen(Name);
		}
	}
}

//Rückgabe
//false=Rohstoff gegen vorhandenen Tauschen
//true=Rohstoff der Datenbank aus xml Datei hinzufügen
bool QExport::AbfrageRohstoffuebernahme(QString str, QString rohstoff)
{
	//Dialog mit Rohstoffauswahl zum Austauschen des zu löschenden Rohstoffes anzeigen
	RohstoffAustauschen raDia;

	//Text für Dialog erstellen
	QString text = trUtf8("Der Rohstoff <b>")+str+trUtf8("</b> ist in deiner Rohstoffauswahl nicht vorhanden");
	text += trUtf8("\n\nSoll dieser Eintrag mit der folgenden Auswahl ersetzt werden?\n");
	text += trUtf8("oder der Datenbank hinzugefügt werden.");
	raDia.SetText(text);
	raDia.setButtonCancelVisible(false);
	raDia.setButtonRohstoffUebernehmenVisible(true);

	//Auswahl für Ersetzung füllen
	QSqlQuery query;
	QString sql = "SELECT * FROM "+rohstoff+" ORDER BY Beschreibung ASC";
	if (!query.exec(sql)) {
		// Fehlermeldung Datenbankabfrage
		ErrorMessage *errorMessage = new ErrorMessage();
		errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
			CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
			+ trUtf8("\nSQL Befehl:\n") + sql);
	}
	else {
		while (query.next()){
			//Beschreibung
			int FeldNr = query.record().indexOf("Beschreibung");
			raDia.addAuswahlEintrag(query.value(FeldNr).toString());
		}
	}

	raDia.exec();
	QCoreApplication::processEvents();

	if (raDia.b_rohstoff_xml_uebernehmen) {
		return true;
	}
	else {
		AustauschRohstoff = raDia.GetAktAuswahl();
		return false;
	}

	
}

void QExport::HinweisAusgeben(QString Text)
{
	QMessageBox msgBox;
	msgBox.setWindowTitle(trUtf8("Hinweis!"));
	msgBox.setText(Text);
	msgBox.setIcon(QMessageBox::Information);

	msgBox.exec();

	//while (QCoreApplication::hasPendingEvents())
	QCoreApplication::processEvents();
}


void QExport::RohstoffMalzUebernehmen(QString Name)
{
	QDomDocument doc("mydoc");
	QFile file(Dateiname);
	if (!file.open(QIODevice::ReadOnly)){
		//return 1; //Datei kann nicht geöffnet werden
	}
	if (!doc.setContent(&file, &errMsg, &errLine, &errCol)) {
		file.close(); //Fehler beim parsen der xml Datei
		//return 2;
	}
	file.close();
	QDomElement root = doc.documentElement();
	if (root.tagName() != "xsud") {
		errMsg = trUtf8("Die Datei ist keine gültige xsud Importdatei");
	}
	else {
		//Malzdaten auslesen
		QDomElement Rohstoffe = root.firstChildElement("Rohstoffe");
		if (!Rohstoffe.isNull()) {
			QDomElement Malz = Rohstoffe.firstChildElement("Malz");
			if (!Malz.isNull()) {
				QDomNode n = Malz.firstChild();
				while(!n.isNull()) {
					QDomNode Eintrag = n;
					QString s;
					if(!Eintrag.isNull()) {
						//Wenn der Eintrag der gesuchte Rohstoff ist in Datenbank eintragen
						QDomElement e = Eintrag.firstChildElement("Beschreibung");
						if (!e.isNull()){
							if(e.text() == Name){
								QSqlQuery query;
								QString s;
								QString sql = "INSERT INTO Malz(Beschreibung, Farbe, MaxProzent, Menge, Preis, Bemerkung, Anwendung, Eingelagert, Mindesthaltbar) VALUES(";
								//Beschreibung
								e = Eintrag.firstChildElement("Beschreibung");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s.replace("'","''") + "',";
								//Malzfarbwert
								e = Eintrag.firstChildElement("Farbe");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Maximal empfohlener Schuettungsanteil
								e = Eintrag.firstChildElement("MaxProzent");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Menge
								s = "0";
								sql += "'" + s + "',";
								//Einkaufspreis
								e = Eintrag.firstChildElement("Preis");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Bemerkung
								e = Eintrag.firstChildElement("Bemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//Anwendung
								e = Eintrag.firstChildElement("Anwendung");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Eingelagert
								e = Eintrag.firstChildElement("Eingelagert");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Mindesthaltbar
								e = Eintrag.firstChildElement("Mindesthaltbar");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "'";

								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
						}
					}
					n = n.nextSibling();
				}
			}
		}
	}
}

void QExport::RohstoffWeitereZutatUebernehmen(QString Name)
{
	QDomDocument doc("mydoc");
	QFile file(Dateiname);
	if (!file.open(QIODevice::ReadOnly)){
		//return 1; //Datei kann nicht geöffnet werden
	}
	if (!doc.setContent(&file, &errMsg, &errLine, &errCol)) {
		file.close(); //Fehler beim parsen der xml Datei
		//return 2;
	}
	file.close();
	QDomElement root = doc.documentElement();
	if (root.tagName() != "xsud") {
		errMsg = trUtf8("Die Datei ist keine gültige xsud Importdatei");
	}
	else {
		//Weitere Zutaten auslesen
		QDomElement Rohstoffe = root.firstChildElement("Rohstoffe");
		if (!Rohstoffe.isNull()) {
			QDomElement WeitereZutaten = Rohstoffe.firstChildElement("WeitereZutaten");
			if (!WeitereZutaten.isNull()) {
				QDomNode n = WeitereZutaten.firstChild();
				while(!n.isNull()) {
					QDomNode Eintrag = n;
					QString s;
					if(!Eintrag.isNull()) {
						//Wenn der Eintrag der gesuchte Rohstoff ist in Datenbank eintragen
						QDomElement e = Eintrag.firstChildElement("Beschreibung");
						if (!e.isNull()){
							if(e.text() == Name){
								QSqlQuery query;
								QString s;
								QString sql = "INSERT INTO WeitereZutaten(Beschreibung, Menge, Einheiten, Typ, Ausbeute, EBC, Preis, Bemerkung) VALUES(";
								//Beschreibung
								e = Eintrag.firstChildElement("Beschreibung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//Vorhandene Menge
								s = "0";
								sql += "'" + s + "',";
								//Einheiten Typ 0=Kg 1=g
								e = Eintrag.firstChildElement("Einheiten");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Art der Zutat 0=Honig 1=Zucker 2=Gewuerz 3=Frucht 4=Sonstiges 100=Hopfen
								e = Eintrag.firstChildElement("Typ");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Ausbeute 0= Keine Auswirkung auf die Stammwürze
								e = Eintrag.firstChildElement("Ausbeute");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Farbe 0=Keine Auswirkung auf die Farbe
								e = Eintrag.firstChildElement("EBC");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Einkaufspreis
								e = Eintrag.firstChildElement("Preis");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Bemerkung
								e = Eintrag.firstChildElement("Bemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "'";
								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
						}
					}
					n = n.nextSibling();
				}
			}
		}
	}
}



void QExport::RohstoffHopfenUebernehmen(QString Name)
{
	QDomDocument doc("mydoc");
	QFile file(Dateiname);
	if (!file.open(QIODevice::ReadOnly)){
		//return 1; //Datei kann nicht geöffnet werden
	}
	if (!doc.setContent(&file, &errMsg, &errLine, &errCol)) {
		file.close(); //Fehler beim parsen der xml Datei
		//return 2;
	}
	file.close();
	QDomElement root = doc.documentElement();
	if (root.tagName() != "xsud") {
		errMsg = trUtf8("Die Datei ist keine gültige xsud Importdatei");
	}
	else {
		//Hopfendaten auslesen
		QDomElement Rohstoffe = root.firstChildElement("Rohstoffe");
		if (!Rohstoffe.isNull()) {
			QDomElement Malz = Rohstoffe.firstChildElement("Hopfen");
			if (!Malz.isNull()) {
				QDomNode n = Malz.firstChild();
				while(!n.isNull()) {
					QDomNode Eintrag = n;
					QString s;
					if(!Eintrag.isNull()) {
						//Wenn der Eintrag der gesuchte Rohstoff ist in Datenbank eintragen
						QDomElement e = Eintrag.firstChildElement("Beschreibung");
						if (!e.isNull()){
							if(e.text() == Name){
								QSqlQuery query;
								QString s;
								QString sql = "INSERT INTO Hopfen(Beschreibung, Alpha, Menge, Preis, Pellets, Bemerkung, Eigenschaften, Typ, Eingelagert, Mindesthaltbar) VALUES(";
								//Beschreibung
								e = Eintrag.firstChildElement("Beschreibung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//Alphaprozentgehalt
								e = Eintrag.firstChildElement("Alpha");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Vorhandene Menge
								s = "0";
								sql += "'" + s + "',";
								//Einkaufspreis
								e = Eintrag.firstChildElement("Preis");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Hopfenart 1=Pellets
								e = Eintrag.firstChildElement("Pellets");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Bemerkung
								e = Eintrag.firstChildElement("Bemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//Eigenschaften
								e = Eintrag.firstChildElement("Eigenschaften");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//Typ
								e = Eintrag.firstChildElement("Typ");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Eingelagert
								e = Eintrag.firstChildElement("Eingelagert");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Mindesthaltbar
								e = Eintrag.firstChildElement("Mindesthaltbar");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "'";

								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
						}
					}
					n = n.nextSibling();
				}
			}
		}
	}
}




void QExport::RohstoffHefeUebernehmen(QString Name)
{
	QDomDocument doc("mydoc");
	QFile file(Dateiname);
	if (!file.open(QIODevice::ReadOnly)){
		//return 1; //Datei kann nicht geöffnet werden
	}
	if (!doc.setContent(&file, &errMsg, &errLine, &errCol)) {
		file.close(); //Fehler beim parsen der xml Datei
		//return 2;
	}
	file.close();
	QDomElement root = doc.documentElement();
	if (root.tagName() != "xsud") {
		errMsg = trUtf8("Die Datei ist keine gültige xsud Importdatei");
	}
	else {
		//Hefedaten auslesen
		QDomElement Rohstoffe = root.firstChildElement("Rohstoffe");
		if (!Rohstoffe.isNull()) {
			QDomElement Malz = Rohstoffe.firstChildElement("Hefe");
			if (!Malz.isNull()) {
				QDomNode n = Malz.firstChild();
				while(!n.isNull()) {
					QDomNode Eintrag = n;
					QString s;
					if(!Eintrag.isNull()) {
						//Wenn der Eintrag der gesuchte Rohstoff ist in Datenbank eintragen
						QDomElement e = Eintrag.firstChildElement("Beschreibung");
						if (!e.isNull()){
							if(e.text() == Name){
								QSqlQuery query;
								QString s;
								QString sql = "INSERT INTO Hefe(Beschreibung, Menge, Einheiten, Preis, Bemerkung, Verpackungsmenge, TypOGUG, TypTrFl, Temperatur, Eigenschaften, SED, EVG, Eingelagert, Mindesthaltbar) VALUES(";
								//Beschreibung
								e = Eintrag.firstChildElement("Beschreibung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//Vorhandene Menge
								s = "0";
								sql += "'" + s + "',";
								//Benoetigte Einheiten
								e = Eintrag.firstChildElement("Einheiten");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Einkaufspreis
								e = Eintrag.firstChildElement("Preis");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Bemerkung
								e = Eintrag.firstChildElement("Bemerkung");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//Verpackungsmenge
								e = Eintrag.firstChildElement("Verpackungsmenge");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//TypOGUG
								e = Eintrag.firstChildElement("TypOGUG");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//TypTrFl
								e = Eintrag.firstChildElement("TypTrFl");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Temperatur
								e = Eintrag.firstChildElement("Temperatur");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Eigenschaften
								e = Eintrag.firstChildElement("Eigenschaften");
								if (!e.isNull())
									s = e.text().replace("'","''");
								else
									s = "";
								sql += "'" + s + "',";
								//SED
								e = Eintrag.firstChildElement("SED");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//EVG
								e = Eintrag.firstChildElement("EVG");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Eingelagert
								e = Eintrag.firstChildElement("Eingelagert");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "',";
								//Mindesthaltbar
								e = Eintrag.firstChildElement("Mindesthaltbar");
								if (!e.isNull())
									s = e.text();
								else
									s = "";
								sql += "'" + s + "'";
								sql += ");";
								if (!query.exec(sql)) {
									// Fehlermeldung Datenbankabfrage
									ErrorMessage *errorMessage = new ErrorMessage();
									errorMessage -> showMessage(ERR_SQL_DB_ABFRAGE, TYPE_WARNUNG,
										CANCEL_NO, trUtf8("Rückgabe:\n") + query.lastError().databaseText()
										+ trUtf8("\nSQL Befehl:\n") + sql);
								}
							}
						}
					}
					n = n.nextSibling();
				}
			}
		}
	}
}
