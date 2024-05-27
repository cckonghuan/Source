/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Library.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__LIBRARY_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/


/********************************************************************************
* Constants																		*
********************************************************************************/


/********************************************************************************
* External variables															*
********************************************************************************/


/********************************************************************************
* External routine prototypes													*
********************************************************************************/


/********************************************************************************
* Input variables																*
********************************************************************************/


/********************************************************************************
* Output variables																*
********************************************************************************/
INT32U CRC32_TABLE[256] = {0};


/********************************************************************************
* Internal variables															*
********************************************************************************/


/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
INT8U   sbDigitExtract(INT32U dwNumber,INT8U bPosition);
INT8U   sbDigitExtract_16Hex(INT32U dwNumber,INT8U bPosition);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
INT8U	sbGetBmsMode(void)
{
	return bBmsMode;
}

/********************************************************************************
* Function name:sbDigitExtract                                                  *
* Parameters:   dwNumber:   Number to be converted into ASCII code              *
*               bPosition:                                                      *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       bCode:      Ascii code converted from number                    *
* Description:  Convert the secleced digital to Ascii code                      *
********************************************************************************/
INT8U   sbDigitExtract(INT32U dwNumber,INT8U bPosition)
{
    INT8U   bCode;

    switch(bPosition)
    {
        case 1:
        default:
            bCode = dwNumber % 10 + '0';
            break;
        case 2:
            bCode = (dwNumber / 10) % 10 + '0';
            break;
        case 3:
            bCode = (dwNumber / 100) % 10 + '0';
            break;
        case 4:
            bCode = (dwNumber / 1000) % 10 + '0';
            break;
        case 5:
            bCode = (dwNumber / 10000) + '0';
            break;
    }
    return(bCode);
}

/********************************************************************************
* The function is internal subrountine. Don't be called by external subrountine *
********************************************************************************/
/********************************************************************************
* Function name:sbDigitExtract_16Hex                                                  *
* Parameters:   dwNumber:  Number to be converted into ASCII code              *
*               bPosition:                                                      *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       bCode:      Ascii code converted from number                    *
* Description:  Convert the 16Hex secleced digital to Ascii code                      *
********************************************************************************/
INT8U   sbDigitExtract_16Hex(INT32U dwNumber,INT8U bPosition)
{
	INT8U	bCode;

	switch(bPosition)
	{
		case 1:
		default:
			bCode = dwNumber % 16;
			if(bCode > 9)
			{
				bCode = bCode + 0x37;
			}
			else
			{
				bCode = bCode + '0';
			}
			break;
		case 2:
			bCode = (dwNumber / 16) % 16;
			if(bCode > 9)
			{
				bCode = bCode + 0x37;
			}
			else
			{
				bCode = bCode + '0';
			}
			break;
		case 3:
			bCode = (dwNumber / 256) % 16;
			if(bCode > 9)
			{
				bCode = bCode + 0x37;
			}
			else
			{
				bCode = bCode + '0';
			}
			break;
		case 4:
			bCode = (dwNumber / 4096) % 16;
			if(bCode > 9)
			{
				bCode = bCode + 0x37;
			}
			else
			{
				bCode = bCode + '0';
			}
			break;
	}
	return(bCode);
}

/********************************************************************************
* Library module useful functions declaration for other tasks                   *
* Following functions can be executed in every task                             *
********************************************************************************/

/********************************************************************************
* Function name:swRoot                                                          *
* Parameters:   dwNumber                                                        *
* Global vars:                                                                  *
* Local vars:   i:      Counter for approaching loop                            *
*               dwSquareRoot:   Temporary register of Square Root               *
* Return:       Root of dwNumber                                                *
* Description:  1.1 dwNumber < 65536*64 ==> Guess: dwNumber / 1024 + 63         *
*               1.2 dwNumber < 65536*2048 ==> Guess: dwNumber / 4096 + 255      *
*               1.3 else ==> Guess: dwNumber / 16384 + 1023                     *
*               2.  Y1 = ((dwNumber / Y0 + Y0) / 2                              *
********************************************************************************/
INT16U  swRoot(INT32U dwNumber)
{
    INT8U   i;
    INT32U  dwSquareRoot;

    if(dwNumber == 0)
    {
        return(0);
    }

    if(dwNumber <= 4194304)
    {
        dwSquareRoot = (dwNumber / 1024) + 63;
    }
    else if(dwNumber <= 134217728)
    {
        dwSquareRoot = (dwNumber / 4096) + 255;
    }
    else
    {
        dwSquareRoot = (dwNumber / 16384) + 1023;
    }

    for(i = 0; i < 5; i++)
    {
        dwSquareRoot = (dwSquareRoot + (dwNumber / dwSquareRoot)) / 2;
    }

    return((INT16U)dwSquareRoot);
}

/********************************************************************************
* Function name:sb8UOutRangeChk                                                 *
* Parameters:   wCompareData:   Data to be compared                             *
*               wHighRange:     High limit range                                *
*               wLowRange:      Low Limit range                                 *
*               bFiler:         Number of times to be confirmed                 *
*               pbCounter:      Pointer of Counter register                     *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       true:           1 ( The data to be compared is out of range )   *
*               false:          0                                               *
* Description:  Check if input data is out of range                             *
********************************************************************************/
INT8U   sb8UOutRangeChk(INT16S wCompareData,INT16S wHighRange,INT16S wLowRange,INT8U bFilter,INT8U *pbCounter)
{
    if((wCompareData > wHighRange) || (wCompareData < wLowRange))
    {
        (*pbCounter)++;
    }
    else
    {
        (*pbCounter) = 0;
    }

    if((*pbCounter) >= bFilter)
    {
        (*pbCounter) = 0;
        return(true);
    }
    else
    {
        return(false);
    }
}

/********************************************************************************
* Function name:sb8UInRangeChk                                                  *
* Parameters:   wCompareData:   Data to be compared                             *
*               wHighRange:     High limit range                                *
*               wLowRange:      Low Limit range                                 *
*               bFiler:         Number of times to be confirmed                 *
*               pbCounter:      Pointer of Counter register                     *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       true:           1 ( The data to be compared is in range )       *
*               false:          0                                               *
* Description:  Check if input data is in range                                 *
********************************************************************************/
INT8U   sb8UInRangeChk(INT16S wCompareData,INT16S wHighRange,INT16S wLowRange,INT8U bFilter,INT8U *pbCounter)
{
    if((wCompareData > wHighRange) || (wCompareData < wLowRange))
    {
        (*pbCounter) = 0;
    }
    else
    {
        (*pbCounter)++;
    }

    if((*pbCounter) >= bFilter)
    {
        (*pbCounter) = 0;
        return(true);
    }
    else
    {
        return(false);
    }
}

/********************************************************************************
* Function name:sb16UOutRangeChk                                                *
* Parameters:   wCompareData:   Data to be compared                             *
*               wHighRange:     High limit range                                *
*               wLowRange:      Low Limit range                                 *
*               wFiler:         Number of times to be confirmed                 *
*               pwCounter:      Pointer of Counter register                     *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       true:           1 ( The data to be compared is out of range )   *
*               false:          0                                               *
* Description:  Check if input data is out of range                             *
********************************************************************************/
INT8U   sb16UOutRangeChk(INT16S wCompareData,INT16S wHighRange,INT16S wLowRange,INT16U wFiler,INT16U *pwCounter)
{
    if((wCompareData > wHighRange) || (wCompareData < wLowRange))
    {
        (*pwCounter)++;
    }
    else
    {
        (*pwCounter) = 0;
    }

    if((*pwCounter) >= wFiler)
    {
        (*pwCounter) = 0;
        return(true);
    }
    else
    {
        return(false);
    }
}
/********************************************************************************
* Function name:sb16UInRangeChk                                                 *
* Parameters:   wCompareData:   Data to be compared                             *
*               wHighRange:     High limit range                                *
*               wLowRange:      Low Limit range                                 *
*               wFiler:         Number of times to be confirmed                 *
*               pwCounter:      Pointer of Counter register                     *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       true:           1 ( The data to be compared is in range )       *
*               false:          0                                               *
* Description:  Check if input data is in range                                 *
********************************************************************************/
INT8U   sb16UInRangeChk(INT16S wCompareData,INT16S wHighRange,INT16S wLowRange,INT16U wFiler,INT16U *pwCounter)
{
    if((wCompareData > wHighRange) || (wCompareData < wLowRange))
    {
        (*pwCounter) = 0;
    }
    else
    {
        (*pwCounter)++;
    }

    if((*pwCounter) >= wFiler)
    {
        (*pwCounter) = 0;
        return(true);
    }
    else
    {
        return(false);
    }
}

/********************************************************************************
* Function name:sb8UOverLevelChk                                                *
* Parameters:   wCompareData:   Data to be compared                             *
*               wHighLevel:     High limit level                                *
*               bFiler:         Number of times to be confirmed                 *
*               pbCounter:      Pointer of Counter register                     *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       true:           1 ( The data to be compared is over level )     *
*               false:          0                                               *
* Description:  Check if input data is over level                               *
********************************************************************************/
INT8U   sb8UOverLevelChk(INT16S wCompareData,INT16S wHighLevel,INT8U bFilter,INT8U *pbCounter)
{
	if(wCompareData >= wHighLevel)
	{
		(*pbCounter)++;
	}
	else
	{
		(*pbCounter) = 0;
	}

	if((*pbCounter) >= bFilter)
	{
		(*pbCounter) = 0;
		return(true);
	}
	else
	{
		return(false);
	}
}

/********************************************************************************
* Function name:sb8UUnderLevelChk                                               *
* Parameters:   wCompareData:   Data to be compared                             *
*               wLowLevel:      Low limit level                                 *
*               bFiler:         Number of times to be confirmed                 *
*               pbCounter:      Pointer of Counter register                     *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       true:           1 ( The data to be compared is under level )    *
*               false:          0                                               *
* Description:  Check if input data is under level                              *
********************************************************************************/
INT8U   sb8UUnderLevelChk(INT16S wCompareData,INT16S wLowLevel,INT8U bFilter,INT8U *pbCounter)
{
	if(wCompareData <= wLowLevel)
	{
		(*pbCounter)++;
	}
	else
	{
		(*pbCounter) = 0;
	}

	if((*pbCounter) >= bFilter)
	{
		(*pbCounter) = 0;
		return(true);
	}
	else
	{
		return(false);
	}
}

/********************************************************************************
* Function name:sb16UOverLevelChk                                               *
* Parameters:   wCompareData:   Data to be compared                             *
*               wHighLevel:     High limit level                                *
*               wFilter:        Number of times to be confirmed                 *
*               pwCounter:      Pointer of Counter register                     *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       true:           1 ( The data to be compared is over level )     *
*               false:          0                                               *
* Description:  Check if input data is over level                               *
********************************************************************************/
INT8U   sb16UOverLevelChk(INT16S wCompareData,INT16S wHighLevel,INT16U wFilter,INT16U *pwCounter)
{
	if(wCompareData >= wHighLevel)
	{
		(*pwCounter)++;
	}
	else
	{
		(*pwCounter) = 0;
	}

	if((*pwCounter) >= wFilter)
	{
		(*pwCounter) = 0;
		return(true);
	}
	else
	{
		return(false);
	}
}

/********************************************************************************
* Function name:sb16UUnderLevelChk                                              *
* Parameters:   wCompareData:   Data to be compared                             *
*               wLowLevel:      Low limit level                                 *
*               wFilter:        Number of times to be confirmed                 *
*               pwCounter:      Pointer of Counter register                     *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       true:           1 ( The data to be compared is under level )    *
*               false:          0                                               *
* Description:  Check if input data is under level                              *
********************************************************************************/
INT8U   sb16UUnderLevelChk(INT16S wCompareData,INT16S wLowLevel,INT16U wFilter,INT16U *pwCounter)
{
	if(wCompareData <= wLowLevel)
	{
		(*pwCounter)++;
	}
	else
	{
		(*pwCounter) = 0;
	}
	
	if((*pwCounter) >= wFilter)
	{
		(*pwCounter) = 0;
		return(true);
	}
	else
	{
		return(false);
	}
}

/********************************************************************************
* Function name:sb8UCountDown                                                   *
* Parameters:   pbCounter:  Pointer of count down counter                       *
*               (Input parameter must be an INT8U parameter)                    *
* Local vars:                                                                   *
* Return:       true:   1(The counter is counted to 0 or counter is 0 already)  *
*               false:  0                                                       *
* Description:  Counter count down to zero                                      *
********************************************************************************/
INT8U   sb8UCountDown(INT8U *pbCounter)
{
    if((*pbCounter) == 0)
    {
        return(true);
    }
    else
    {
        (*pbCounter)--;
        if((*pbCounter) == 0)
        {
            return(true);
        }
        else
        {
            return(false);
        }
    }
}

/********************************************************************************
* Function name:sb16UCountDown                                                  *
* Parameters:   pwCounter:  Pointer of count down counter                       *
*               (Input parameter must be an INT16U parameter)                   *
* Local vars:                                                                   *
* Return:       true:   1(The counter is counted to 0 or counter is 0 already)  *
*               false:  0                                                       *
* Description:  Counter count down to zero                                      *
********************************************************************************/
INT8U   sb16UCountDown(INT16U *pwCounter)
{
    if((*pwCounter) == 0)
    {
        return(true);
    }
    else
    {
        (*pwCounter)--;
        if((*pwCounter) == 0)
        {
            return(true);
        }
        else
        {
            return(false);
        }
    }
}

/********************************************************************************
* Function name:sb32UCountDown                                                  *
* Parameters:   pdwCounter: Pointer of count down counter                       *
*               (Input parameter must be an INT32U parameter)                   *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       true:   1(The counter is counted to 0 or counter is 0 already)  *
*               false:  0                                                       *
* Description:  Counter count down to zero                                      *
********************************************************************************/
INT8U   sb32UCountDown(INT32U *pdwCounter)
{
    if((*pdwCounter) == 0)
    {
        return(true);
    }
    else
    {
        (*pdwCounter)--;
        if((*pdwCounter) == 0)
        {
            return(true);
        }
        else
        {
            return(false);
        }
    }
}

/********************************************************************************
* Function name:sbNumToX                                                        *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:   Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 9,             *
*                           and limited to 9)                                   *
* Description:  Convert input wNumber into 1 digital numbers represented by     *
*               ASCII code.                                                     *
********************************************************************************/
INT8U   sbNumToX(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 9)
    {
        wNumFormated = 9;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract(wNumFormated,1);
    *(pbBuffer + 1) = '\0';
    return(bConvResult);
}

/********************************************************************************
* Function name:sbNumToXx                                                       *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:      Number after formatting                      *
*               bConvResult:       Conversion result                            *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 9.9,           *
*                           and limited to 9.9)                                 *
* Description:  Convert input wNumber into 1 digital numbers including 1 decimal*
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXx(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 99)
    {
        wNumFormated = 99;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract(wNumFormated,2);
    *(pbBuffer + 1) = '.';
    *(pbBuffer + 2) = sbDigitExtract(wNumFormated,1);
    *(pbBuffer + 3) = '\0';
    return(bConvResult);
}

/********************************************************************************
* Function name:sbNumToXxx                                                      *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:   Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 9.99,          *
*                           and limited to 9.99)                                *
* Description:  Convert input wNumber into 1 digital numbers including 2 decimal*
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXxx(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 999)
    {
        wNumFormated = 999;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract(wNumFormated,3);
    *(pbBuffer + 1) = '.';
    *(pbBuffer + 2) = sbDigitExtract(wNumFormated,2);
    *(pbBuffer + 3) = sbDigitExtract(wNumFormated,1);
    *(pbBuffer + 4) = '\0';
    return(bConvResult);
}

/********************************************************************************
* Function name:sbNumToXxxx                                                     *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:       Number after formatting                     *
*               bConvResult:        Conversion result                           *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 9.999,         *
*                           and limited to 9.999)                               *
* Description:  Convert input wNumber into 1 digital numbers including 3 decimal*
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXxxx(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 9999)
    {
        wNumFormated = 9999;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract(wNumFormated,4);
    *(pbBuffer + 1) = '.';
    *(pbBuffer + 2) = sbDigitExtract(wNumFormated,3);
    *(pbBuffer + 3) = sbDigitExtract(wNumFormated,2);
    *(pbBuffer + 4) = sbDigitExtract(wNumFormated,1);
    *(pbBuffer + 5) = '\0';
    return(bConvResult);
}


/********************************************************************************
* Function name:sbNumToXxxxx                                                    *
* Parameters:   dwNumber:   Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   dwNumFormated:      Number after formatting                     *
*               bConvResult:        Conversion result                           *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 9.9999,        *
*                           and limited to 9.9999)                              *
* Description:  Convert input wNumber into 1 digital numbers including 4 decimal*
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXxxxx(INT32U dwNumber,INT8U *pbBuffer)
{
    INT32U  dwNumFormated;
    INT8U   bConvResult = true;

    if(dwNumber > 99999)
    {
        dwNumFormated = 99999;
        bConvResult = false;
    }
    else
    {
        dwNumFormated = dwNumber;
    }
    *(pbBuffer) = sbDigitExtract(dwNumFormated,5);
    *(pbBuffer + 1) = '.';
    *(pbBuffer + 2) = sbDigitExtract(dwNumFormated,4);
    *(pbBuffer + 3) = sbDigitExtract(dwNumFormated,3);
    *(pbBuffer + 4) = sbDigitExtract(dwNumFormated,2);
    *(pbBuffer + 5) = sbDigitExtract(dwNumFormated,1);
    *(pbBuffer + 6) = '\0';
    return(bConvResult);
}


/********************************************************************************
* Function name:sbNumToXX                                                       *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:   Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 99,            *
*                           and limited to 99)                                  *
* Description:  Convert input wNumber into 2 digital numbers                    *
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXX(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 99)
    {
        wNumFormated = 99;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract(wNumFormated,2);
    *(pbBuffer + 1) = sbDigitExtract(wNumFormated,1);
    *(pbBuffer + 2) = '\0';
    return(bConvResult);
}

INT8U   sbNumToXX_16Hex(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 0xFF)
    {
        wNumFormated = 0xFF;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract_16Hex(wNumFormated,2);
    *(pbBuffer + 1) = sbDigitExtract_16Hex(wNumFormated,1);
    *(pbBuffer + 2) = '\0';
    return(bConvResult);
}

/********************************************************************************
* Function name:sbNumToXXx                                                      *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:   Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 99.9,          *
*                           and limited to 99.9)                                *
* Description:  Convert input wNumber into 2 digital numbers including 1 decimal*
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXXx(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 999)
    {
        wNumFormated = 999;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract(wNumFormated,3);
    *(pbBuffer + 1) = sbDigitExtract(wNumFormated,2);
    *(pbBuffer + 2) = '.';
    *(pbBuffer + 3) = sbDigitExtract(wNumFormated,1);
    *(pbBuffer + 4) = '\0';
    return(bConvResult);
}


/********************************************************************************
* Function name:sbNumToXXxx                                                     *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:   Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 99.99,         *
*                           and limited to 99.99)                               *
* Description:  Convert input wNumber into 2 digital numbers including 2 decimal*
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXXxx(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 9999)
    {
        wNumFormated = 9999;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract(wNumFormated,4);
    *(pbBuffer + 1) = sbDigitExtract(wNumFormated,3);
    *(pbBuffer + 2) = '.';
    *(pbBuffer + 3) = sbDigitExtract(wNumFormated,2);
    *(pbBuffer + 4) = sbDigitExtract(wNumFormated,1);
    *(pbBuffer + 5) = '\0';
    return(bConvResult);
}


/********************************************************************************
* Function name:sbNumToXXxxx                                                    *
* Parameters:   dwNumber:   Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   dwNumFormated:  Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 99.999,        *
*                           and limited to 99.999)                              *
* Description:  Convert input wNumber into 2 digital numbers including 3 decimal*
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXXxxx(INT32U dwNumber,INT8U *pbBuffer)
{
    INT32U  dwNumFormated;
    INT8U   bConvResult = true;

    if(dwNumber > 99999)
    {
        dwNumFormated = 99999;
        bConvResult = false;
    }
    else
    {
        dwNumFormated = dwNumber;
    }
    *(pbBuffer) = sbDigitExtract(dwNumFormated,5);
    *(pbBuffer + 1) = sbDigitExtract(dwNumFormated,4);
    *(pbBuffer + 2) = '.';
    *(pbBuffer + 3) = sbDigitExtract(dwNumFormated,3);
    *(pbBuffer + 4) = sbDigitExtract(dwNumFormated,2);
    *(pbBuffer + 5) = sbDigitExtract(dwNumFormated,1);
    *(pbBuffer + 6) = '\0';
    return(bConvResult);
}


/********************************************************************************
* Function name:sbNumToXXX                                                      *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:   Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 999,           *
*                           and limited to 999)                                 *
* Description:  Convert input wNumber into 3 digital numbers                    *
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXXX(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 999)
    {
        wNumFormated = 999;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract(wNumFormated,3);
    *(pbBuffer + 1) = sbDigitExtract(wNumFormated,2);
    *(pbBuffer + 2) = sbDigitExtract(wNumFormated,1);
    *(pbBuffer + 3) = '\0';
    return(bConvResult);
}


/********************************************************************************
* Function name:sbNumToXXXx                                                     *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:   Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 999.9,         *
*                           and limited to 999.9)                               *
* Description:  Convert input wNumber into 3 digital numbers including 1 decimal*
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXXXx(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 9999)
    {
        wNumFormated = 9999;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract(wNumFormated,4);
    *(pbBuffer + 1) = sbDigitExtract(wNumFormated,3);
    *(pbBuffer + 2) = sbDigitExtract(wNumFormated,2);
    *(pbBuffer + 3) = '.';
    *(pbBuffer + 4) = sbDigitExtract(wNumFormated,1);
    *(pbBuffer + 5) = '\0';
    return(bConvResult);
}


/********************************************************************************
* Function name:sbNumToXXXxx                                                    *
* Parameters:   dwNumber:   Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   dwNumFormated:  Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 999.99,        *
*                           and limited to 999.99)                              *
* Description:  Convert input wNumber into 3 digital numbers including 2 decimal*
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXXXxx(INT32U dwNumber,INT8U *pbBuffer)
{
    INT32U  dwNumFormated;
    INT8U   bConvResult = true;

    if(dwNumber > 99999)
    {
        dwNumFormated = 99999;
        bConvResult = false;
    }
    else
    {
        dwNumFormated = dwNumber;
    }
    *(pbBuffer) = sbDigitExtract(dwNumFormated,5);
    *(pbBuffer + 1) = sbDigitExtract(dwNumFormated,4);
    *(pbBuffer + 2) = sbDigitExtract(dwNumFormated,3);
    *(pbBuffer + 3) = '.';
    *(pbBuffer + 4) = sbDigitExtract(dwNumFormated,2);
    *(pbBuffer + 5) = sbDigitExtract(dwNumFormated,1);
    *(pbBuffer + 6) = '\0';
    return(bConvResult);
}


/********************************************************************************
* Function name:sbNumToXXXX                                                     *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:   Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 9999,          *
*                           and limited to 9999)                                *
* Description:  Convert input wNumber into 4 digital numbers                    *
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXXXX(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 9999)
    {
        wNumFormated = 9999;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract(wNumFormated,4);
    *(pbBuffer + 1) = sbDigitExtract(wNumFormated,3);
    *(pbBuffer + 2) = sbDigitExtract(wNumFormated,2);
    *(pbBuffer + 3) = sbDigitExtract(wNumFormated,1);
    *(pbBuffer + 4) = '\0';
    return(bConvResult);
	
}
/********************************************************************************
* Function name:sbNumToXXXX_16Hex                                                     *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   wNumFormated:   Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 0xFFFF,          *
*                           and limited to 0xFFFF)                                *
* Description:  Convert input wNumber into 4 digital numbers                    *
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXXXX_16Hex(INT16U wNumber,INT8U *pbBuffer)
{
    INT16U  wNumFormated;
    INT8U   bConvResult = true;

    if(wNumber > 0xFFFF)
    {
        wNumFormated = 0xFFFF;
        bConvResult = false;
    }
    else
    {
        wNumFormated = wNumber;
    }
    *(pbBuffer) = sbDigitExtract_16Hex(wNumFormated,4);
    *(pbBuffer + 1) = sbDigitExtract_16Hex(wNumFormated,3);
    *(pbBuffer + 2) = sbDigitExtract_16Hex(wNumFormated,2);
    *(pbBuffer + 3) = sbDigitExtract_16Hex(wNumFormated,1);
    *(pbBuffer + 4) = '\0';
    return(bConvResult);
}

/********************************************************************************
* Function name:sbNumToXXXXx                                                    *
* Parameters:   dwNumber:   Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   dwNumFormated:  Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 9999.9,        *
*                           and limited to 9999.9)                              *
* Description:  Convert input wNumber into 4 digital numbers including 1 decimal*
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXXXXx(INT32U dwNumber,INT8U *pbBuffer)
{
    INT32U  dwNumFormated;
    INT8U   bConvResult = true;

    if(dwNumber > 99999)
    {
        dwNumFormated = 99999;
        bConvResult = false;
    }
    else
    {
        dwNumFormated = dwNumber;
    }
    *(pbBuffer) = sbDigitExtract(dwNumFormated,5);
    *(pbBuffer + 1) = sbDigitExtract(dwNumFormated,4);
    *(pbBuffer + 2) = sbDigitExtract(dwNumFormated,3);
    *(pbBuffer + 3) = sbDigitExtract(dwNumFormated,2);
    *(pbBuffer + 4) = '.';
    *(pbBuffer + 5) = sbDigitExtract(dwNumFormated,1);
    *(pbBuffer + 6) = '\0';
    return(bConvResult);
}


/********************************************************************************
* Function name:sbNumToXXXXX                                                    *
* Parameters:   dwNumber:   Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   dwNumFormated:  Number after formatting                         *
*               bConvResult:    Conversion result                               *
* Return:       true:       1(The Number to be converted is correction)         *
*               false:      0(The Number to be converted is over 99999,         *
*                           and limited to 99999)                               *
* Description:  Convert input wNumber into 5 digital numbers                    *
*               represented by ASCII code                                       *
********************************************************************************/
INT8U   sbNumToXXXXX(INT32U dwNumber,INT8U *pbBuffer)
{
    INT32U  dwNumFormated;
    INT8U   bConvResult = true;

    if(dwNumber > 99999)
    {
        dwNumFormated = 99999;
        bConvResult = false;
    }
    else
    {
        dwNumFormated = dwNumber;
    }
    *(pbBuffer) = sbDigitExtract(dwNumFormated,5);
    *(pbBuffer + 1) = sbDigitExtract(dwNumFormated,4);
    *(pbBuffer + 2) = sbDigitExtract(dwNumFormated,3);
    *(pbBuffer + 3) = sbDigitExtract(dwNumFormated,2);
    *(pbBuffer + 4) = sbDigitExtract(dwNumFormated,1);
    *(pbBuffer + 5) = '\0';
    return(bConvResult);
}


/********************************************************************************
* Function name:sNumToAscii                                                     *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   bIndex:     for loop index                                      *
*               bPick:      for loop index                                      *
* Return:                                                                       *
* Description:  Convert input wNumber into digital ASCII code and suppress      *
*               leading zero,ONLY deal with number between 0~65535              *
********************************************************************************/
void    sNumToAscii(INT16U wNumber,INT8U *pbBuffer)
{
    INT8U   bIndex,bPick;

    for(bIndex = 5; bIndex > 1; bIndex--)
    {
        if(sbDigitExtract(wNumber,bIndex) != '0')
        {
            break;
        }
    }

    for(bPick = 0; bPick < bIndex; bPick++)
    {
        *(pbBuffer + bPick) = sbDigitExtract(wNumber,(bIndex - bPick));
    }

    *(pbBuffer + bPick) = '\0';
}

/********************************************************************************
* Function name:sNumToAscii                                                     *
* Parameters:   wNumber:    Number to be converted into ASCII code              *
*               pbBuffer:   Pointer of conversion register                      *
* Global vars:                                                                  *
* Local vars:   bIndex:     for loop index                                      *
*               bPick:      for loop index                                      *
* Return:                                                                       *
* Description:  Convert input wNumber into digital ASCII code and suppress      *
*               leading zero,ONLY deal with number between 0~65535              *
********************************************************************************/
void    sNumToAscii_16Hex(INT16U wNumber,INT8U *pbBuffer)
{
	INT8U	bIndex,bPick;

	for(bIndex = 4; bIndex > 1; bIndex--)
	{
		if(sbDigitExtract_16Hex(wNumber,bIndex) != '0')
		{
			break;
		}
	}

	for(bPick = 0; bPick < bIndex; bPick++)
	{
		*(pbBuffer + bPick) = sbDigitExtract_16Hex(wNumber,(bIndex - bPick));
	}

	*(pbBuffer + bPick) = '\0';
}

/********************************************************************************
* Function name:sbAsciiToX                                                      *
* Parameters:   pbBuffer:   Start pointer of conversion register                *
* Global vars:                                                                  *
* Local vars:   bLength:    Number to convert                                   *
* Return:       bNumberTemp:    The accumulated number                          *
*               c8UDataErr:     Value isn't '0'~'9' in Ascii format             *
* Description:  Convert input Ascii code into number and add up 1 number        *
********************************************************************************/
INT8U   sbAsciiToX(const INT8U *pbBuffer)
{
    INT8U   bNumberTemp = 0;
    INT8U   bLength = 1;

    while(bLength != 0)
    {
        if((*pbBuffer < '0') || (*pbBuffer > '9'))
        {
            return(c8UDataErr);
        }

        bNumberTemp = bNumberTemp * 10 + (*pbBuffer++ - '0');
        bLength--;
    }
    return(bNumberTemp);
}


/********************************************************************************
* Function name:sbAsciiToXX                                                     *
* Parameters:   pbBuffer:   Start pointer of conversion register                *
* Global vars:                                                                  *
* Local vars:   bLength:    Number to convert                                   *
* Return:       bNumberTemp:    The accumulated number                          *
*               c8UDataErr:     Value isn't '0'~'9' in Ascii format             *
* Description:  Convert input Ascii code into number and add up 2 number        *
********************************************************************************/
INT8U   sbAsciiToXX(const INT8U *pbBuffer)
{
    INT8U   bNumberTemp = 0;
    INT8U   bLength = 2;

    while(bLength != 0)
    {
        if((*pbBuffer < '0') || (*pbBuffer > '9'))
        {
            return(c8UDataErr);
        }

        bNumberTemp = bNumberTemp * 10 + (*pbBuffer++ - '0');
        bLength--;
    }
    return(bNumberTemp);
}


/********************************************************************************
* Function name:swAsciiToXXX                                                    *
* Parameters:   pbBuffer:   Start pointer of conversion register                *
* Global vars:                                                                  *
* Local vars:   bLength:    Number to convert                                   *
* Return:       wNumberTemp:    The accumulated number                          *
*               c16UDataErr:    Value isn't '0'~'9' in Ascii format             *
* Description:  Convert input Ascii code into number and add up 3 number        *
********************************************************************************/
INT16U  swAsciiToXXX(const INT8U *pbBuffer)
{
    INT16U  wNumberTemp = 0;
    INT8U   bLength = 3;

    while(bLength != 0)
    {
        if((*pbBuffer < '0') || (*pbBuffer > '9'))
        {
            return(c16UDataErr);
        }

        wNumberTemp = wNumberTemp * 10 + (*pbBuffer++ - '0');
        bLength--;
    }
    return(wNumberTemp);
}


/********************************************************************************
* Function name:swAsciiToXXXX                                                   *
* Parameters:   pbBuffer:   Start pointer of conversion register                *
* Global vars:                                                                  *
* Local vars:   bLength:    Number to convert                                   *
* Return:       wNumberTemp:    The accumulated number                          *
*               c16UDataErr:    Value isn't '0'~'9' in Ascii format             *
* Description:  Convert input Ascii code into number and add up 4 number        *
********************************************************************************/
INT16U  swAsciiToXXXX(const INT8U *pbBuffer)
{
    INT16U  wNumberTemp = 0;
    INT8U   bLength = 4;

    while(bLength != 0)
    {
        if((*pbBuffer < '0') || (*pbBuffer > '9'))
        {
            return(c16UDataErr);
        }

        wNumberTemp = wNumberTemp * 10 + (*pbBuffer++ - '0');
        bLength--;
    }
    return(wNumberTemp);
}


/********************************************************************************
* Function name:sdwAsciiToXXXXX                                                 *
* Parameters:   pbBuffer:   Start pointer of conversion register                *
* Global vars:                                                                  *
* Local vars:   bLength:    Number to convert                                   *
* Return:       dwNumberTemp:   The accumulated number                          *
*               c32UDataErr:    Value isn't '0'~'9' in Ascii format             *
* Description:  Convert input Ascii code into number and add up 5 number        *
********************************************************************************/
INT32U  sdwAsciiToXXXXX(const INT8U *pbBuffer)
{
    INT32U  dwNumberTemp = 0;
    INT8U   bLength = 5;

    while(bLength != 0)
    {
        if((*pbBuffer < '0') || (*pbBuffer > '9'))
        {
            return(c32UDataErr);
        }

        dwNumberTemp = dwNumberTemp * 10 + (*pbBuffer++ - '0');
        bLength--;
    }
    return(dwNumberTemp);
}


/********************************************************************************
* Function name:sdwAsciiToNum                                                   *
* Parameters:   pbBuffer:   Start pointer of conversion register                *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       dwNumberTemp:   the accumulated number                          *
*               c32UDataErr:    Start Value isn't '0'~'9' in Ascii format       *
* Description:  Convert input Ascii code into number and add up number until    *
*               not '0'~'9' is received.                                        *
********************************************************************************/
INT32U  sdwAsciiToNum(const INT8U *pbBuffer)
{
    INT32U  dwNumberTemp = 0;

    if((*pbBuffer < '0') || (*pbBuffer > '9'))
    {
        return(c32UDataErr);
    }

    while((*pbBuffer >= '0') && (*pbBuffer <= '9'))
    {
        dwNumberTemp = dwNumberTemp * 10 + (*pbBuffer++ - '0');
    }

    return(dwNumberTemp);
}


/********************************************************************************
* Function name:sdwAsciiDotxToNum                                               *
* Parameters:   pbBuffer:   Start pointer of conversion register                *
* Global vars:                                                                  *
* Local vars:                                                                   *
* Return:       dwNumberTemp:   the accumulated number                          *
*               c32UDataErr:    Start Value isn't '.','0'~'9' in Ascii format   *
*                           or no any '.' character in conversion register      *
* Description:  Convert input Ascii code into number and add up number until    *
*               not '0'~'9' is received.                                        *
********************************************************************************/
INT32U  sdwAsciiDotxToNum(const INT8U *pbBuffer)
{
    INT32U  dwNumberTemp = 0;
    INT8U   bLength = 1;

    if(((*pbBuffer < '0') || (*pbBuffer > '9')) && (*pbBuffer != '.'))
    {
        return(c32UDataErr);
    }

    while((*pbBuffer >= '0') && (*pbBuffer <= '9'))
    {
        dwNumberTemp = dwNumberTemp * 10 + (*pbBuffer++ - '0');
    }

    if(*pbBuffer == '.')
    {
        pbBuffer++;
        while(bLength != 0)
        {
            if((*pbBuffer < '0') || (*pbBuffer > '9'))
            {
                return(c32UDataErr);
            }

            dwNumberTemp = dwNumberTemp * 10 + (*pbBuffer++ - '0');
            bLength--;
        }
        return(dwNumberTemp);
    }
    return(c32UDataErr);
}

/********************************************************************************
* Function name:sbAbs                                                           *
* Parameters:   dwData1:	Data to be subtracted              					*
* Global vars:  None                                                            *
* Local vars:   None															*
* Return:       None															*
* Description:  Two 32bit number subduction,use for HCS08 microchip				*
********************************************************************************/
INT8U	sbAbs(INT8S bData)
{
	if(bData < 0)
	{
		return -bData;
	}
	else
	{
		return bData;
	}
}

/********************************************************************************
* Function name:swAbs                                                           *
* Parameters:   dwData1:	Data to be subtracted              					*
* Global vars:  None                                                            *
* Local vars:   None															*
* Return:       None															*
* Description:  Two 32bit number subduction,use for HCS08 microchip				*
********************************************************************************/
INT16U	swAbs(INT16S wData)
{
	if(wData < 0)
	{
		return -wData;
	}
	else
	{
		return wData;
	}
}

INT16U	swHighLowByteSwap(INT16U wData)
{
	INT8U *pSrc, *pDes;
	INT16U temp;
	
	pDes = (INT8U *)&temp;
	pSrc = (INT8U *)&wData;
	*pDes = *(pSrc+1);
	*(pDes+1) = *pSrc;
	
	return temp;
}

INT8U	sbHighBitNum8(INT8U value)	// 计算bit为1的个数
{
	value = (value & 0x55) + ((value >> 1) & 0x55);
	value = (value & 0x33) + ((value >> 2) & 0x33);
	value = (value & 0x0F) + ((value >> 4) & 0x0F);
	
	return value;
}

INT8U	sbHighBitNum16(INT16U value)
{
	value = (value & 0x5555) + ((value >> 1) & 0x5555);
	value = (value & 0x3333) + ((value >> 2) & 0x3333);
	value = (value & 0x0F0F) + ((value >> 4) & 0x0F0F);
	value = (value & 0x00FF) + ((value >> 8) & 0x00FF);
	
	return value;
}

INT8U	sbHighBitNum32(INT32U value)
{
	value = (value & 0x55555555) + ((value >> 1) & 0x55555555);
	value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
	value = (value & 0x0F0F0F0F) + ((value >> 4) & 0x0F0F0F0F);
	value = (value & 0x00FF00FF) + ((value >> 8) & 0x00FF00FF);
	value = (value & 0x0000FFFF) + ((value >> 16)& 0x0000FFFF);
	
	return value;
}

INT8U	sbFirstBitHighNo8(INT8U value)	// 计算第一个bit为1的位置
{
	value = (value - 1) & ~value;
	value = (value & 0x55) + ((value >> 1) & 0x55);
	value = (value & 0x33) + ((value >> 2) & 0x33);
	value = (value & 0x0F) + ((value >> 4) & 0x0F);
	
	return value;
}

INT8U	sbFirstBitHighNo16(INT16U value)
{
	value = (value - 1) & ~value;
	value = (value & 0x5555) + ((value >> 1) & 0x5555);
	value = (value & 0x3333) + ((value >> 2) & 0x3333);
	value = (value & 0x0F0F) + ((value >> 4) & 0x0F0F);
	value = (value & 0x00FF) + ((value >> 8) & 0x00FF);
	
	return value;
}

INT8U	sbFirstBitHighNo32(INT32U value)
{
	value = (value - 1) & ~value;
	value = (value & 0x55555555) + ((value >> 1) & 0x55555555);
	value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
	value = (value & 0x0F0F0F0F) + ((value >> 4) & 0x0F0F0F0F);
	value = (value & 0x00FF00FF) + ((value >> 8) & 0x00FF00FF);
	value = (value & 0x0000FFFF) + ((value >> 16)& 0x0000FFFF);
	
	return value;
}

void	sMemorySet(INT8U *pt, INT8U setval, INT8U length)
{
	INT8U i;
	for(i=0; i<length; i++)
	{
		*pt = setval;
		pt++;
	}
}

void	sMemoryCopy(INT8U *source, INT8U *target, INT8U length)
{
	INT8U i;
	for(i=0; i<length; i++)
	{
		*target = *source;
		target++;
		source++;
	}
}

INT16U	swHighLowSwap(INT16U *pData)
{
	INT16U	wTemp;
	
	wTemp = (*pData) >> 8;
	wTemp += (*pData) << 8;
	return wTemp;
}

void	sInvertUint8(INT8U *dBuf, INT8U *srcBuf)
{
	INT8U i;
	INT8U tmp;
	
	tmp = 0;
	for(i = 0; i < 8; i++)
	{
		if(*srcBuf & ((INT8U)1 << i))
		{
			tmp |= (INT8U)1 << (7 - i);
		}
	}
	*dBuf = tmp;
}

void	sInvertUint16(INT16U *dBuf, INT16U *srcBuf)
{
	INT8U i;
	INT16U tmp;
	
	tmp = 0;
	for(i = 0; i < 16; i++)
	{
		if(*srcBuf & ((INT16U)1 << i))
		{
			tmp |= (INT16U)1 << (15 - i);
		}
	}
	*dBuf = tmp;
}

INT16U	swModbusCrc16(INT8U *puchMsg, INT8U ucDataLen)
{
	INT16U usRCin = 0xFFFF;
	INT16U usCPoly = 0x8005;
	INT8U ucChar = 0, i;
	
	while(ucDataLen--)	
	{
		ucChar = *(puchMsg++);
		sInvertUint8(&ucChar, &ucChar);
		usRCin ^= ((INT16U)ucChar << 8);
		for(i = 0; i < 8; i++)
		{
			if(usRCin & 0x8000)
			{
				usRCin = (usRCin << 1) ^ usCPoly;
			}
			else
			{
				usRCin = usRCin << 1;
			}
		}
	}
	
	sInvertUint16(&usRCin, &usRCin);
	
	ucChar = usRCin >> 8;
	return(usRCin << 8 | ucChar);
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
void sGetCrc32Table(void)  //生成输入、输出反转的表 
{
	INT32U i , j;
	INT32U CRCValue = 0;
	for(i = 0 ; i < 256 ; i++)
	{
		CRCValue = i;
		for(j = 0 ; j < 8 ; j++)
		{
			if(CRCValue & 0x00000001)
			{
				CRCValue = (CRCValue >> 1) ^ 0xEDB88320;
			}
			else
			{
				CRCValue >>= 1;
			}
		}
		CRC32_TABLE[i] = CRCValue;
	}
} 


INT32U sxCrc32Check(INT8U *datptr , INT8U len , INT32U InitValue)
{
	INT32U CRC32 = InitValue;
	CRC32 = ~CRC32;
	while(len--)
	{
		CRC32 =  (CRC32 >> 8) ^ CRC32_TABLE[((*datptr++) ^ CRC32) & 0xFF];
	}
	return CRC32 ^ 0xFFFFFFFF;
} 


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


