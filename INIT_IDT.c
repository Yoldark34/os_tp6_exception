#include "PLAN_MEMOIRE.h"
#include "Descripteurs.h"
#include "Init_IDT.h"
#include "EXCEPTIONS.h"
#include "MACRO.h"


//------------------------------------------------------------------------------
// Variables globale
//------------------------------------------------------------------------------
T_INFO_IDT Info_IDT;
//------------------------------------------------------------------------------

typedef struct
{
    BYTE   Interruption;  // numero d'interrupt /exception
    UINT32 Code;
}T_ASSOCIATION_INT_CODE;
//------------------------------------------------------------------------------

T_ASSOCIATION_INT_CODE Association_Exception[]={
    {EXCEPT_ERREUR_DIVISION, (UINT32)Exception_Erreur_Division},
    {EXCEPT_MISE_AU_POINT, (UINT32)Exception_Mise_Au_Point},
    {EXCEPT_INT3, (UINT32)Exception_INT3},
    {EXCEPT_DEBORDEMENT, (UINT32)Exception_Debordement},
    {EXCEPT_LIMITE, (UINT32)Exception_Limite},
    {EXCEPT_CODEOP_INVALIDE, (UINT32)Exception_CodeOP_Invalide},
    {EXCEPT_DNA, (UINT32)Exception_DNA},
    {EXCEPT_DOUBLE_FAUTE, (UINT32)Exception_Double_Faute},
    {EXCEPT_DEPASSEMENT, (UINT32)Exception_Depassement},
    {EXCEPT_TSS_INVALIDE, (UINT32)Exception_TSS_Invalide},
    {EXCEPT_SEGMENT_NON_PRESENT, (UINT32)Exception_Segment_Non_Present},
    {EXCEPT_SEGMENT_PILE, (UINT32)Exception_Segment_Pile},
    {EXCEPT_VIOLATION_PROTECTION, (UINT32)Exception_Violation_Protection},
    {EXCEPT_DEFAUT_DE_PAGE, (UINT32)Exception_Defaut_De_Page},
    {EXCEPT_COPROCESSEUR, (UINT32)Exception_CoProcesseur}
};



//==============================================================================
void Configuration_Descripteurs_Exceptions()
{
T_DESCRIPTEUR_GUICHET* L_Descripteurs_Guichet=(T_DESCRIPTEUR_GUICHET*)ADRESSE_BASE_DESCRIPTEUR_GUICHET;
UINT16 L_Index;
   for (L_Index=0; L_Index<NOMBRE_EXCEPTIONS; L_Index++) {
        Configure_Descripteur_Guichet(&L_Descripteurs_Guichet[Association_Exception[L_Index].Interruption],
                                       SELECTEUR_CODE_NOYAU,                // selecteur du segment de code
                                      Association_Exception[L_Index].Code,
                                      GUICHET_INT_386,
                                      RING_0,    // Privilege 0
                                      1);  // Bit_P présent
    }
}


//==============================================================================
void Initialisation_IDT()
{
T_DESCRIPTEUR_GUICHET* L_Descripteurs_Guichet=(T_DESCRIPTEUR_GUICHET*)ADRESSE_BASE_DESCRIPTEUR_GUICHET;

UINT16 L_Index;
  // Définir un comportemnt déterministe pour toutes les exceptions
   for (L_Index=0; L_Index<LIMITE_DESCRIPTEURS_GUICHET; L_Index++) {
        Configure_Descripteur_Guichet(&L_Descripteurs_Guichet[L_Index],
                                       SELECTEUR_CODE_NOYAU,
                                      (UINT32)Exception_Par_Defaut,
                                      GUICHET_INT_386,
                                      RING_0,    // Privilege 0
                                      1);  // Bit_P présent
    }
 //.............................................................................
 Configuration_Descripteurs_Exceptions();

 //.............................................................................

 Info_IDT.Base =ADRESSE_BASE_DESCRIPTEUR_GUICHET ;
 Info_IDT.Limite=LIMITE_DESCRIPTEURS_GUICHET*sizeof(T_DESCRIPTEUR_GUICHET);

  INITIALISER_REGISTRE_IDTR(Info_IDT);
}

