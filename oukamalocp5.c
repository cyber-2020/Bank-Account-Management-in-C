/*
            **************************************************************************************************************
            Purpose - Manage and maintain accounts for a local bank                       
            **************************************************************************************************************
            Data Files
            cp50950.txt - input data file containing info about the account,
                          output a report as istructed in the problem statement
            oukamlopcp5.out - output file to which results will be written 
            **************************************************************************************************************/
            
/*Preprocessor Directives*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
    
/* struct transaction stores transaction information for the account */
/* it has trans.number, type, date, payee, amount fields */
/*it links back itself by the next(pointer to struct transaction) field */
    struct transaction{
        int transNumber;
        char transType[5];
        int transDate;
        char transPayee[30];
        double transAmount;
        struct transaction *next;
    };
    typedef struct transaction TRANS;
  
/*struct account stores account holder name, card number, account balance*/
/*it links to the TRANS structure type with the start field */
    struct accountt{
        char name[30];
        int num;
        double balance;
        TRANS *start; 
    };
    typedef struct accountt ACCOUNT;

/* function prototypes for all the necessary functions */ 

ACCOUNT StartAccount(FILE* datFilePtr);
int GetTrans(FILE* datFilePtr, FILE* outFilePtr, ACCOUNT* accountPtr);
TRANS* ReadTrans( FILE* datFilePtr, int transNum );
int InsertTrans( ACCOUNT* accountPtr, TRANS* newTransPtr );
void GenerateReport(FILE* outFilePtr, ACCOUNT* curAccount);
int DeleteTrans(ACCOUNT* curAccount, int transNum);


/* main function starts here */
int main()
{
/* *************************************************************************************************** */
/*                  Begin by opening input, output files & writing title                               */
/* *************************************************************************************************** */
    /* open files for reading and writing*/
	FILE* datFilePtr = fopen("cp50950.txt","r");
	FILE* outFilePtr = fopen("oukamalocp5.out", "w");

    fprintf(outFilePtr,"*******************************************************************************\n");
    fprintf(outFilePtr,"Name - \n");
    fprintf(outFilePtr,"*******************************************************************************\n");
    
	/* create new account */
	ACCOUNT custAccount = StartAccount(datFilePtr);    
    
    /* read file for transactions */
	while(GetTrans(datFilePtr, outFilePtr, &custAccount)!= EOF);
    
    fprintf(outFilePtr,"\nE n d   of   r e p o r t !\n\n\n");
    fprintf(outFilePtr,"*******************************************************************************\n\n\n");
        
    /* close input & output files */
	fclose(datFilePtr);
	fclose(outFilePtr);

	return EXIT_SUCCESS;
}    /* end of main */            

/*function startAccount starts here, it reads the account holder name, card number, */
/*and balance from file and returns it to the main function */
ACCOUNT StartAccount(FILE* datFilePtr)
{
    ACCOUNT *AccountInfo;
    AccountInfo=(ACCOUNT *)malloc(sizeof(ACCOUNT));
    fscanf(datFilePtr,"%[^\n]%d%lf",AccountInfo->name,&AccountInfo->num,&AccountInfo->balance);
    AccountInfo->start=NULL;
    return *AccountInfo;
}

/*function GetTrans start here, it reads the next transaction number from the data file and*/
/*determines which course of action to take (add transaction, delete transaction, or */
/*print report).*/
int GetTrans(FILE* datFilePtr, FILE* outFilePtr, ACCOUNT* accountPtr)
{
    int transNum;         /*used to input transaction #*/
    TRANS *newTransPtr;   /*used to point the newly read in struct transaction node*/

    if(fscanf(datFilePtr,"%d",&transNum)==1) /*read in the next transaction number*/
    {
        if(transNum>0) /*reads next 4 lines from the input file & inserts into the account*/
        {
             newTransPtr=ReadTrans(datFilePtr,transNum);
             InsertTrans(accountPtr,newTransPtr);
        }
        else if(transNum<0) /*deletes the corresponding transaction from the account*/
        {
             DeleteTrans(accountPtr,fabs(transNum));
        }
        else /*if transNum==0, generates the current report for the account*/
        {
            GenerateReport(outFilePtr,accountPtr);
        }
        return EXIT_SUCCESS;

    }
    else
    {
        return EOF; /*return end of file when there is no more values in the input file*/    
    }   
}

/*function Read Trans reads transaction information from the data file and */
/*creates a new transaction to insert in the account list.*/
TRANS* ReadTrans(FILE* datFilePtr, int transNum)
{
      /*allocate dynamic memory to store values that are inputted*/
      TRANS *ptr=(TRANS*)malloc(sizeof(TRANS));
	  
	  /*ptr points to the struct transaction, transNumber member field*/
      ptr->transNumber=transNum;
      
	  /*read in 4 valued for the corresponding transaction Number ('transNum')*/
      fscanf(datFilePtr,"%s",ptr->transType);
      fscanf(datFilePtr,"%d\n",&ptr->transDate);
      fscanf(datFilePtr,"%[^\n]",ptr->transPayee);
      fscanf(datFilePtr,"%lf",&ptr->transAmount);
      
	  return ptr; 
}

/* function InsertTrans inserts a transaction into the account list according to*/
/*its transaction date. The account list should be sorted by ascending */
/*transaction dates when this operation is finished.It returns the value */
/*EXIT_SUCCESS if the function completes execution.*/
int InsertTrans(ACCOUNT* accountPtr, TRANS* newTransPtr)
{
      TRANS *prev = NULL;                      /* prev point to NULL*/
      TRANS *curr = accountPtr->start;
      TRANS *newnode;
               
      int found = 0;    
      
      /* go through list until insertion point or end of list is found*/
      while (curr != NULL && !found) 
      {
		 /*if used to locate where in account we need to input the */
		 /*newly read in transaction information*/
         if((newTransPtr->transDate)<(curr->transDate)) 
            found = 1;
         else  
         {
            prev = curr;
            curr = curr->next;
         }
      }

      /*create memory to dynamically store the newly read in transaction info*/
      newnode = (TRANS*)malloc (sizeof(TRANS));
      newnode = newTransPtr;
      newTransPtr->next = curr;
   
      /*connect the subsequent nodes(point one's end to the address of te other)*/
      if (prev == NULL) 
         accountPtr->start = newnode;
      else 
         prev->next = newnode;
      
      return EXIT_SUCCESS;
}

/*generates an account statement as shown in the example output and writes */
/*this information to an output file pointed to by outFilePtr. The output */
/*should begin by displaying the account information (i.e., name of customer,*/
/*account number, beginning balance). Afterwards, for each transaction, print one*/
/*line with all fields in the transaction except for the transaction number.*/ 
void GenerateReport(FILE* outFilePtr, ACCOUNT* curAccount)
{
     TRANS *ptr2; /*ptr2 used to access and output the members of the transaction*/
	 
	 /*output the account holder info*/
	 fprintf (outFilePtr, "\n%s\n",curAccount->name);
	 fprintf (outFilePtr, "%d\n",curAccount->num);
	 fprintf (outFilePtr, "%.2f\n\n",curAccount->balance);

     /*points ptr2 to the start of the transactions(right after account holder info)*/
     ptr2 = curAccount->start;
     double total=(curAccount->balance);
     while(ptr2!=NULL) /*loop through while we are not at the end of the account*/
	 {  
        total=total+(ptr2->transAmount); /*calculate the current account balance*/
        
        /*output the current transactions for the account*/    
        fprintf(outFilePtr,"%5s   ",ptr2->transType);
    	fprintf(outFilePtr,"%d   ",ptr2->transDate);
    	fprintf(outFilePtr,"%-30s   ",ptr2->transPayee);
    	fprintf(outFilePtr,"%10.2f   ",ptr2->transAmount);
    	fprintf(outFilePtr,"%10.2f\n",total);
    	ptr2=ptr2->next; /*point to the next node*/
     }  
     

}

/*function DeleteTrans This function deletes a transaction from the account*/
/*list and updates all node pointers accordingly. The list should */
/*remain sorted after performing this operation. It returns the value */
/*EXIT_SUCCESS if the function completes execution.*/
int DeleteTrans(ACCOUNT* curAccount, int transNum)
{
    /*create TRANS type pointers to manipulate through the nodes*/
    /*prev - points to the previous node, curr - points to the corrent node*/
    TRANS   *prev = NULL,
            *curr = curAccount->start; /*point current to the start of transactions*/
    
    /*used as an exit flag to determine whether we have match for the transaction*/
    /*Number that has been read from the file, 0 indicates no, 1 indicates yes*/
    int found =0;   
    while((curr != NULL)&&(!found))   /*used to find the match*/
    {
        if((curr->transNumber) == transNum)   /*if the match is found*/
            found = 1;
        else   /*if the match is not found*/
        {
            prev = curr;
            curr = curr->next;
        }
    }
        
    if(found==1)
    {
    if(prev == NULL)
        curAccount->start = curr->next;
    else
        prev->next = curr->next;
    free(curr);
    return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}

/* **************************************************************************************************** */
/*                                      END   OF   CODE                                                          */
/* **************************************************************************************************** */
