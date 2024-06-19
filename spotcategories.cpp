#include "spotcategories.h"

QString SpotCategories::cat2name(int cat, const QByteArray &subcat)
 {
     if (subcat.length() >= 2)
     {
         char str2 = subcat[0];
         int  str3 = subcat.mid(1).toInt();

         switch (cat)
         {
             case 1:
             case 9:
             {
                 char str2 = subcat[0];
                 if (str2 == 'b')
                 {
                     switch (str3)
                     {
                         case 0:
                             return tr("CAM");

                         case 1:
                             return tr("(S)VCD");

                         case 2:
                             return tr("Promo");

                         case 3:
                             return tr("Retail");

                         case 4:
                             return tr("TV");

                         case 5:
                             return "";

                         case 6:
                             return tr("Satelliet");

                         case 7:
                             return tr("R5");

                         case 8:
                             return tr("Telecine");

                         case 9:
                             return tr("Telesync");

                        case 10:
                             return tr("Scan");
                         default:
                             break;
                     }
                 }
                 else if (str2 == 'c')
                 {
                     switch (str3)
                     {
                         case 0:
                             return tr("Geen ondertitels");

                         case 1:
                             return tr("Nederlands ondertiteld (extern)");

                         case 2:
                             return tr("Nederlands ondertiteld (ingebakken)");

                         case 3:
                             return tr("Engels ondertiteld (extern)");

                         case 4:
                             return tr("Engels ondertiteld (ingebakken)");

                         case 5:
                             return "";

                         case 6:
                             return tr("Nederlands ondertiteld (instelbaar)");

                         case 7:
                             return tr("Engels ondertiteld (instelbaar)");

                         case 8:
                         case 9:
                             break; /* ??? */

                         case 10:
                             return tr("Engels gesproken");

                         case 11:
                             return tr("Nederlands gesproken");

                         case 12:
                             return tr("Duits gesproken");

                         case 13:
                             return tr("Frans gesproken");

                         case 14:
                             return tr("Spaans gesproken");

                         default:
                             break;
                     }
                 }
                 else if (str2 == 'd')
                 {
                     switch (str3)
                     {
                         case 0:
                             return tr("Actie");

                         case 1:
                             return tr("Avontuur");

                         case 2:
                             return tr("Animatie");

                         case 3:
                             return tr("Cabaret");

                         case 4:
                             return tr("Komedie");

                         case 5:
                             return tr("Misdaad");

                         case 6:
                             return tr("Documentaire");

                         case 7:
                             return tr("Drama");

                         case 8:
                             return tr("Familiefilm");

                         case 9:
                             return tr("Fantasiefilm");

                         case 10:
                             return tr("Film Noir");

                         case 11:
                             return tr("TV Series");

                         case 12:
                             return tr("Horror");

                         case 13:
                             return tr("Muziek");

                         case 14:
                             return tr("Musical");

                         case 15:
                             return tr("Mysterie");

                         case 0x10:
                             return tr("Romantiek");

                         case 0x11:
                             return tr("Science Fiction");

                         case 0x12:
                             return tr("Sport");

                         case 0x13:
                             return tr("Korte film");

                         case 20:
                             return tr("Thriller");

                         case 0x15:
                             return tr("Oorlog");

                         case 0x16:
                             return tr("Western");

                         case 0x17:
                             return tr("Erotiek (hetero)");

                         case 0x18:
                             return tr("Erotiek (gay mannen)");

                         case 0x19:
                             return tr("Erotiek (gay vrouwen)");

                         case 0x1a:
                             return tr("Erotiek (bi)");

                         case 0x1b:
                             return "";

                         case 0x1c:
                             return tr("Asian");

                         case 0x1d:
                             return tr("Anime");

                         case 30:
                             return tr("Cover");

                         case 0x1f:
                             return tr("Comics");

                         case 0x20:
                             return tr("Cartoons");

                         case 0x21:
                             return tr("Kinderfilm");

                         /* Nieuw */
                         case 0x22:
                             return tr("Zakelijk");

                         case 0x23:
                             return tr("Computer");

                         case 0x24:
                             return tr("Hobby");

                         case 0x25:
                             return tr("Koken");

                         case 0x26:
                             return tr("Knutselen");

                         case 0x27:
                             return tr("Handwerk");

                         case 0x28:
                             return tr("Gezondheid");

                         case 0x29:
                             return tr("Geschiedenis");

                         case 0x2a:
                             return tr("Psychologie");

                         case 0x2b:
                             return tr("Krant");

                         case 0x2c:
                             return tr("Tijdschrift");

                         case 0x2d:
                             return tr("Wetenschap");

                         case 0x2e:
                             return tr("Vrouw");

                         case 0x2f:
                             return tr("Religie");

                         case 0x30:
                             return tr("Roman");

                         case 0x31:
                             return tr("Biografie");

                         case 0x32:
                             return tr("Detective");

                         case 0x33:
                             return tr("Dieren");

                         case 0x35:
                             return tr("Reizen");

                         case 0x36:
                             return tr("Waargebeurd");

                         case 0x37:
                             return tr("Non-fictie");

                         case 0x39:
                             return tr("Poezie");

                         case 0x3a:
                             return tr("Sprookje");

                         case 0x48:
                             return tr("Bi");

                         case 0x49:
                             return tr("Lesbo");

                         case 0x4a:
                             return tr("Homo");

                         case 0x4b:
                             return tr("Hetero");

                         case 0x4c:
                             return tr("Amateur");

                         case 0x4d:
                             return tr("Groep");

                         case 0x4e:
                             return tr("POV");

                         case 0x4f:
                             return tr("Solo");

                         case 80:
                             return tr("Jong");

                         case 0x51:
                             return tr("Soft");

                         case 0x52:
                             return tr("Fetisj");

                         case 0x53:
                             return tr("Oud");

                         case 0x54:
                             return tr("BBW");

                         case 0x55:
                             return tr("SM");

                         case 0x56:
                             return tr("Hard");

                         case 0x57:
                             return tr("Donker");

                         case 0x58:
                             return tr("Hentai");

                         case 0x59:
                             return tr("Buiten");
                         /* --- */

                         default:
                             return tr("Onbekend");
                     }
                 }
                 else if (str2 == 'a')
                 {
                     switch (str3)
                     {
                         case 0:
                             return tr("DivX");

                         case 1:
                             return tr("WMV");

                         case 2:
                             return tr("MPG");

                         case 3:
                             return tr("DVD5");

                         case 4:
                             return tr("HD Overig");

                         case 5:
                             return tr("eBook");

                         case 6:
                             return tr("Blu-ray");

                         case 7:
                             return tr("HD-DVD");

                         case 8:
                             return tr("WMVHD");

                         case 9:
                             return tr("x264HD");

                         case 10:
                             return tr("DVD9");

                         default:
                             break;
                     }
                 }
             }
             break;

             case 2:
             {
                 switch (str2)
                 {
                    case 'a':
                         switch (str3)
                         {
                             case 0:
                                 return tr("MP3");

                             case 1:
                                 return tr("WMA");

                             case 2:
                                 return tr("WAV");

                             case 3:
                                 return tr("OGG");

                             case 4:
                                 return tr("Lossless");

                             case 5:
                                 return tr("DTS");

                             case 6:
                                 return tr("AAC");

                             case 7:
                                 return tr("APE");

                             case 8:
                                 return tr("FLAC");
                             default:
                                 break;
                         }
                     break;

                     case 'b':
                         switch (str3)
                         {
                             case 0:
                                 return tr("CD");

                             case 1:
                                 return tr("Radio");

                             case 2:
                                 return tr("Compilatie");

                             case 3:
                                 return tr("DVD");

                             case 4:
                                 return tr("");

                             case 5:
                                 return tr("Vinyl");

                             case 6:
                                 return tr("Stream");
                             default:
                                 break;
                         }
                     break;

                     case 'c':
                         switch (str3)
                         {
                             case 0:
                                 return tr("Variabel");

                             case 1:
                                 return tr("< 96kbit");

                             case 2:
                                 return tr("96kbit");

                             case 3:
                                 return tr("128kbit");

                             case 4:
                                 return tr("160kbit");

                             case 5:
                                 return tr("192kbit");

                             case 6:
                                 return tr("256kbit");

                             case 7:
                                 return tr("320kbit");

                             case 8:
                                 return tr("Lossless");

                             case 9:
                                 return tr("");

                             default:
                                 break;
                         }
                     break;

                     case 'd':
                         switch (str3)
                         {
                             case 0:
                                 return tr("Blues/Folk");

                             case 1:
                                 return tr("Compilatie");

                             case 2:
                                 return tr("Cabaret");

                             case 3:
                                 return tr("Dance");

                             case 4:
                                 return tr("Diversen");

                             case 5:
                                 return tr("Hardcore");

                             case 6:
                                 return tr("Internationaal");

                             case 7:
                                 return tr("Jazz");

                             case 8:
                                 return tr("Kinder/Jeugd");

                             case 9:
                                 return tr("Klassiek");

                             case 10:
                                 return tr("Kleinkunst");

                             case 11:
                                 return tr("Nederlands");

                             case 12:
                                 return tr("New Age");

                             case 13:
                                 return tr("Pop");

                             case 14:
                                 return tr("R&B/Soul");

                             case 15:
                                 return tr("Hip hop");

                             case 0x10:
                                 return tr("Reggae");

                             case 0x11:
                                 return tr("Religieus");

                             case 0x12:
                                 return tr("Rock");

                             case 0x13:
                                 return tr("Soundtracks");

                             case 20:
                                 return tr("");

                             case 0x15:
                                 return tr("Jumpstyle");

                             case 0x16:
                                 return tr("Asian");

                             case 0x17:
                                 return tr("Disco");

                             case 0x18:
                                 return tr("Oldskool");

                             case 0x19:
                                 return tr("Metal");

                             case 0x1a:
                                 return tr("Country");

                             case 27:
                                 return tr("Dubstep");
                             case 28:
                                 return tr("Nederhop");
                             case 29:
                                 return tr("DnB");
                             case 30:
                                 return tr("Electro");
                             case 31:
                                 return tr("Folk");
                             case 32:
                                 return tr("Soul");
                             case 33:
                                 return tr("Trance");
                             case 34:
                                 return tr("Balkan");
                             case 35:
                                 return tr("Techno");
                             case 36:
                                 return tr("Ambient");
                             case 37:
                                 return tr("Latin");
                             case 38:
                                 return tr("Live");

                             default:
                                 break;
                         }
                 }
             }
             break;

             case 3:
                 switch (str2)
                 {
                     case 'a':
                         switch (str3)
                         {
                             case 0:
                                 return tr("Windows");

                             case 1:
                                 return tr("Macintosh");

                             case 2:
                                 return tr("Linux");

                             case 3:
                                 return tr("Playstation");

                             case 4:
                                 return tr("Playstation 2");

                             case 5:
                                 return tr("PSP");

                             case 6:
                                 return tr("XBox");

                             case 7:
                                 return tr("XBox 360");

                             case 8:
                                 return tr("Gameboy Advance");

                             case 9:
                                 return tr("Gamecube");

                             case 10:
                                 return tr("Nintendo DS");

                             case 11:
                                 return tr("Nintendo Wii");

                             case 12:
                                 return tr("Playstation 3");

                             case 13:
                                 return tr("Windows Phone");

                             case 14:
                                 return tr("iOs");

                             case 15:
                                 return tr("Android");
                             default:
                                 break;
                         }
                         break;

                     case 'b':
                         switch (str3)
                         {
                             case 0:
                                 return tr("ISO");

                             case 1:
                                 return tr("Rip");

                             case 2:
                                 return tr("DVD");

                             case 3:
                                 return tr("Addon");

                             case 4:
                                 return tr("");
                             default:
                                 break;
                         }
                         break;

                     case 'c':
                         switch (str3)
                         {
                             case 0:
                                 return tr("Actie");

                             case 1:
                                 return tr("Avontuur");

                             case 2:
                                 return tr("Strategie");

                             case 3:
                                 return tr("Rollenspel");

                             case 4:
                                 return tr("Simulatie");

                             case 5:
                                 return tr("Race");

                             case 6:
                                 return tr("Vliegen");

                             case 7:
                                 return tr("Shooter");

                             case 8:
                                 return tr("Platform");

                             case 9:
                                 return tr("Sport");

                             case 10:
                                 return tr("Kinder/jeugd");

                             case 11:
                                 return tr("Puzzel");

                             case 12:
                                 return tr("");

                             case 13:
                                 return tr("Bordspel");
                             case 14:
                                 return tr("Kaarten");
                             case 15:
                                 return tr("Educatie");
                             case 16:
                                 return tr("Muziek");
                             case 17:
                                 return tr("Party");

                             default:
                                 break;
                         }
                         break;
                 }

             break;
             case 4:
                 switch (str2)
                 {
                     case 'a':
                         switch (str3)
                         {
                             case 0:
                                 return tr("Windows");

                             case 1:
                                 return tr("Macintosh");

                             case 2:
                                 return tr("Linux");

                             case 3:
                                 return tr("OS/2");

                             case 4:
                                 return tr("Windows Phone");

                             case 5:
                                 return tr("Navigatiesystemen");

                             case 6:
                                 return tr("iOs");

                             case 7:
                                 return tr("Android");
                             default:
                                 break;
                         }
                         break;

                     case 'b':
                         switch (str3)
                         {
                             case 0:
                                 return tr("Audio bewerking");

                             case 1:
                                 return tr("Video bewerking");

                             case 2:
                                 return tr("Grafisch design");

                             case 3:
                                 return tr("CD/DVD Tools");

                             case 4:
                                 return tr("Media spelers");

                             case 5:
                                 return tr("Rippers & Encoders");

                             case 6:
                                 return tr("Plugins");

                             case 7:
                                 return tr("Database tools");

                             case 8:
                                 return tr("Email software");

                             case 9:
                                 return tr("Fotobewerking");

                             case 10:
                                 return tr("Screensavers");

                             case 11:
                                 return tr("Skin software");

                             case 12:
                                 return tr("Drivers");

                             case 13:
                                 return tr("Browsers");

                             case 14:
                                 return tr("Download managers");

                             case 15:
                                 return tr("File sharing");

                             case 0x10:
                                 return tr("Usenet software");

                             case 0x11:
                                 return tr("RSS Readers");

                             case 0x12:
                                 return tr("FTP software");

                             case 0x13:
                                 return tr("Firewalls");

                             case 20:
                                 return tr("Antivirus software");

                             case 0x15:
                                 return tr("Antispyware software");

                             case 0x16:
                                 return tr("Optimalisatiesoftware");

                             case 0x17:
                                 return tr("Beveiligingssoftware");

                             case 0x18:
                                 return tr("Systeemsoftware");

                             case 0x19:
                                 return tr("");

                             case 26:
                                 return tr("Educatief");

                             case 27:
                                 return tr("Kantoor");

                             case 28:
                                 return tr("Internet");

                             case 29:
                                 return tr("Communicatie");

                             case 30:
                                 return tr("Ontwikkel");

                             case 31:
                                 return tr("Spotnet");

                             default:
                                 break;
                         }
                 }
         }
     }

     return tr("");
 }
