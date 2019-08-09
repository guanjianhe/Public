void task_void(void)
{
    static uint16_t LineNum = 0;
    static uint32_t Counter = 0;

    switch (LineNum)
    {
        case 0:
            while (1)
            {
                if (Finger_WAKE == 1)
                {
                    do
                    {
                        Counter = timer_gettick();

                    LineNum = __LINE__, case __LINE__:
                        if ((timer_gettick() - Counter) >= 10)
                        {
                            break;
                        }

                        return;
                    } while (0);

                    if (Finger_WAKE == 1)
                    {
                        Finger_RST = 1;

                        do
                        {
                            Counter = timer_gettick();

                        LineNum = __LINE__, case __LINE__:
                            if ((timer_gettick() - Counter) >= 300)
                            {
                                break;
                            }

                            return;
                        } while (0);

                        finger_send();
                    }
                    else
                    {
                        LineNum = 0;
                        return;
                    }
                }
                else
                {
                    LineNum = 0;
                    return;
                }


            }
    }
}

