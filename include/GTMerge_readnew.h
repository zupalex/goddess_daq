assert (control.fileActive[whatFile[nextTSpoolIndex]] > 0);

badRead = 1;
while (badRead)
  {
    /* read */

    control.nread++;
//    printf("attemp read # %i..",control.nread);
    st = GTGetDiskEv (whatFile[nextTSpoolIndex], nextTSpoolIndex);

//  if (control.nread > 2286) exit(0);

    /* decompose */

    if (st == 0)
      {
        badRead = GTEvdecompose ((GTEVENT *) & Event[nextTSpoolIndex], (DGSEVENT *) & DGSEvent[nextTSpoolIndex]);
//        printf("DGSEvent[%i].LEDts=%lli ",nextTSpoolIndex,DGSEvent[nextTSpoolIndex].LEDts);
//        printf("oldFileTS[%i]=%lli\n",nextTSpoolIndex,oldFileTS[nextTSpoolIndex]);
      };

#if(0)

    /* debug */

    printf ("TS=%lli from file %i\n", DGSEvent[nextTSpoolIndex].LEDts, whatFile[nextTSpoolIndex]);
    if (DGSEvent[nextTSpoolIndex].LEDts > (long long int) 9129656189)
      exit (0);
#endif

    /* zero suppress */

    if (control.zerosuppress)
      if (DGSEvent[nextTSpoolIndex].ehi < control.zerosuppresslim)
        {
          control.nzeroehi++;
          badRead = 1;
        };


    /* fail--quit, or decompose */

    if (st == 2)
      {
        printf ("failed to read more data from file %i, nextTSpoolIndex=%i\n", whatFile[nextTSpoolIndex],
                nextTSpoolIndex);
        fflush (stdout);
        if (control.fileActive[whatFile[nextTSpoolIndex]])
          control.nOpenFiles--;
        control.fileActive[whatFile[nextTSpoolIndex]] = 0;
        DGSEvent[nextTSpoolIndex].LEDts = ULLONG_MAX;
        if (control.nOpenFiles <= 0)
          {
            printf ("no more data to be read\n");
            goto done;
          };

        /* break out of loop */

        badRead = 0;
      };

    /* zero TS is a scaler, we reassign the last */
    /* know TS to them here and set the tid one higher */

    if (DGSEvent[nextTSpoolIndex].TSTYPE == (long long int) 0)
      {
        DGSEvent[nextTSpoolIndex].LEDts = oldFileTS[nextTSpoolIndex];
        DGSEvent[nextTSpoolIndex].tid++;
      };

#if(0)
    if (badRead == 0)
      if (DGSEvent[nextTSpoolIndex].tpe >= 0 && DGSEvent[nextTSpoolIndex].tpe < MAXTPE)
        if (DGSEvent[nextTSpoolIndex].tid >= 0 && DGSEvent[nextTSpoolIndex].tid < MAXTID)
          nstat.in_hit[DGSEvent[nextTSpoolIndex].tpe][DGSEvent[nextTSpoolIndex].tid]++;
#endif

    /* check time stamp did not jump too far forward or backwards */

    dTS = (long long int) DGSEvent[nextTSpoolIndex].LEDts - oldFileTS[nextTSpoolIndex];


#if(0)
//if(curTS>=(long long int)4231520787) 
    if (control.nread > 205129)
      {
        printf ("DEBUG STOP!!!!\n");
        exit (0);
      }
#endif


    if (dTS > control.dts_max && st != 2)
      {
        nstat.nTSjumprecover_f[whatFile[nextTSpoolIndex]]++;
        if (nstat.nTSjumprecover_f[whatFile[nextTSpoolIndex]] < 10)
          {
            printf ("timestamp jumped forward %lli for file %i;", dTS, whatFile[nextTSpoolIndex]);
            printf ("now=%lli; ", DGSEvent[nextTSpoolIndex].LEDts);
            printf ("old=%lli; ", oldFileTS[nextTSpoolIndex]);
            printf ("nread=%i; ", control.nread);
            printf ("\n");
          };
        printf ("reject TS=%lli; file %i\n", DGSEvent[nextTSpoolIndex].LEDts, whatFile[nextTSpoolIndex]);

        /* force rereads */

        badRead = 1;

#if(1)
        /* local count of these events */

        njf++;

        /* breakout */
        /* Idea is that if this happens too many times,  */
        /* it probably was a 'real' jump in time and  */
        /* we should abort our attempts to catch up again.  */
        /* In that case, we reset the counter and set the  */
        /* oldFileTS to the current TS + defeat the  */
        /* badRead we had set above */

        if (njf > control.dtsfabort)
          {
            njf = 0;
            badRead = 0;
            lltmp = oldFileTS[nextTSpoolIndex] - DGSEvent[nextTSpoolIndex].LEDts;
            printf ("abort jump forward recovery, oldFileTS %lli --> %lli; ", oldFileTS[nextTSpoolIndex],
                    DGSEvent[nextTSpoolIndex].LEDts);
            printf ("DT=%lli\n", -lltmp);
            oldFileTS[nextTSpoolIndex] = DGSEvent[nextTSpoolIndex].LEDts;
          };
#endif
      }
    else if (dTS < control.dts_min && st != 2)
      {
        nstat.nTSjumprecover_b[whatFile[nextTSpoolIndex]]++;

        if (nstat.nTSjumprecover_b[whatFile[nextTSpoolIndex]] < 10)
          {
            printf ("TS jumped back %lli; file %i; ", dTS, whatFile[nextTSpoolIndex]);
            printf ("TS=%lli; ", DGSEvent[nextTSpoolIndex].LEDts);
            printf ("old=%lli; ", oldFileTS[nextTSpoolIndex]);
            printf ("nread=%i, njb=%i; ", control.nread, njb);
            printf ("\n");
          };
        printf ("reject TS=%lli; file %i\n", DGSEvent[nextTSpoolIndex].LEDts, whatFile[nextTSpoolIndex]);


        /* force rereads */

        badRead = 1;

#if(1)
        /* local count of these events */

        njb++;

        /* breakout */
        /* Idea is that if this happens too many times,  */
        /* it probably was a 'real' jump back in time and  */
        /* we should abort our attempts to catch up again.  */
        /* In that case, we reset the counter and set the  */
        /* oldFileTS to the current TS + defeat the  */
        /* badRead we had set above */

        if (njb > control.dtsbabort)
          {
            njb = 0;
            badRead = 0;
            lltmp = oldFileTS[nextTSpoolIndex] - DGSEvent[nextTSpoolIndex].LEDts;
            printf ("abort jump back recovery, oldFileTS %lli --> %lli; ", oldFileTS[nextTSpoolIndex],
                    DGSEvent[nextTSpoolIndex].LEDts);
            printf ("DT=%lli\n", lltmp);
            oldFileTS[nextTSpoolIndex] = DGSEvent[nextTSpoolIndex].LEDts;
          };
#endif
      }
    else
      {
        oldFileTS[nextTSpoolIndex] = DGSEvent[nextTSpoolIndex].LEDts;
        njb = 0;
      };



  }
