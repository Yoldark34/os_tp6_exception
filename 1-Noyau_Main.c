#include "type.h"
#include "Ecran.h"
#include "INIT_IDT.h"
#include "Info_Boot.h"
#include "Alien.h"
#include "Init_GDT.h"
#include "MACRO.h"
#include "PLAN_MEMOIRE.h"
#include "HARD_8042_Clavier.h"
#include "Outils.h"
#include "convert_keyboard.h"

Affiche_Informations_BOOT(T_BOOT_INFO* P_Info) {
	if ((P_Info->Flags & BOOT_INFO_MEMOIRE) == BOOT_INFO_MEMOIRE) {
		Regle_Couleur(BLANC);
		Affiche_Chaine(">>>Memoire detectee : ");
		UINT32 L_Taille_Memoire = P_Info->Adresse_Basse + P_Info->Adresse_Haute + 1024;
		Regle_Couleur(BLEU | LUMINEUX);
		Affiche_Chaine(Entier_Vers_Chaine(L_Taille_Memoire / 1024));
		Affiche_Chaine(" Mo\n");
	}
}

void Affiche_Message(UCHAR* P_Message, UCHAR* P_Etat) {
	Regle_Couleur(BLANC);
	Affiche_Chaine(P_Message);
	Positionne_Curseur(78 - Taille_Chaine(P_Etat), Donne_Curseur_Y());
	Regle_Couleur(VERT | LUMINEUX);
	Affiche_Chaine(P_Etat);
	Affiche_Caractere('\n');
}

void OS_Main() {
	Affiche_Message(">>>Initialisation de la Pile (ESP) : ", "OK");
	Initialisation_IDT();
	Affiche_Message(">>>Initialisation de la IDT : ", "OK");
	Affiche_Chaine("Appuyez sur une touche pour lancer la division par 0\n");
	Attendre_Touche_Relache();

	UINT32 Valeur_1 = 10;
	UINT32 Valeur_2 = 0;
	UINT32 Valeur_3;

	Valeur_3 = Valeur_1 / Valeur_2;
	Affiche_Chaine("\nResultat : ");
	Affiche_Chaine(Entier_Vers_Chaine(Valeur_3));
	Affiche_Caractere('\n');

	Affiche_Chaine("\n\nAppuyez sur une touche pour lancer une violation de protection ou appuyez sur echap pour ignorer cette etape\n");
	UINT16 L_Touche = Attendre_Touche_Relache();
	if (L_Touche != 0x81) {
		asm(".intel_syntax noprefix ");
		asm("jmp 0x50:0x100000");
		asm(".att_syntax noprefix \n");
	}

	Affiche_Chaine("\n\nAppuyez sur une touche pour lancer CodeOP Invalide ou appuyez sur Echap pour ignorer cette utape\n");
	L_Touche = Attendre_Touche_Relache();
	if (L_Touche != 0x81) {
		asm(".intel_syntax noprefix ");
		asm("mov AX,0");
		asm("mov cs, ax");
		asm(".att_syntax noprefix \n");
	}

	BYTE prevColor = Donne_Couleur();
	Regle_Couleur(prevColor);

	while (1) {

		int i = 0;
		long boucle = 5000000L;

		Affiche_Alien_1(VERT);
		for (i = 0; i < boucle; i++) {
			asm("NOP");
		}


		Affiche_Alien_2(VERT);
		for (i = 0; i < boucle / 2; i++) {
			asm("NOP");
		}

	}
}


void OS_Start(T_BOOT_INFO* P_Info) {
	Efface_Ecran();
	//Affiche_Alien_2(VERT);
	Affiche_Informations_BOOT(P_Info);
	//BYTE prevColor = Donne_Couleur();
	Regle_Couleur(10);
	Affiche_Chaine("OK\n");

	if ((P_Info->Flags & BOOT_INFO_MEMOIRE) == BOOT_INFO_MEMOIRE) {
		Affiche_Chaine("\n Adresse Basse (640ko maxi) : ");
		Affiche_Chaine(Entier_Vers_Chaine_Hexa(P_Info->Adresse_Basse, 4));

		Affiche_Chaine("\n Adresse Haute (Au dessus des 1 Mo) : ");
		Affiche_Chaine(Entier_Vers_Chaine_Hexa(P_Info->Adresse_Haute, 4));
		Affiche_Chaine("\n\tIl y a donc : ");

		UINT32 L_Taille_Memoire = P_Info->Adresse_Basse + P_Info->Adresse_Haute + 1024;
		Affiche_Chaine(Entier_Vers_Chaine(L_Taille_Memoire));
		Affiche_Chaine(" Ko de memmoire --> ");
		Affiche_Chaine(Entier_Vers_Chaine(L_Taille_Memoire / 1024));
		Affiche_Chaine(" Mo ");
	}

	if ((P_Info->Flags & BOOT_LIGNE_COMMANDE) == BOOT_LIGNE_COMMANDE) {
		Affiche_Chaine("\n Ligne de commenda passée au noyau : ");
		Affiche_Chaine((UCHAR*) P_Info->Ligne_De_Commande);
	}

	if ((P_Info->Flags & BOOT_INFO_BOOTLOADER) == BOOT_INFO_BOOTLOADER) {
		Affiche_Chaine("\n Nom du bootloader: ");
		Affiche_Chaine((UCHAR*) P_Info->Nom_BootLoader);
	}

	Initialisation_GDT();

	INITIALISE_SS_ESP(SELECTEUR_STACK_NOYAU, DEBUT_STACK_NOYAU);
	OS_Main();
	asm("NOP");
}

