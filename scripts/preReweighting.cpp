/*
Yields for Nominal (1) and MG5Py8 (2)
Sample-1 - Total weight for TTL 168079
Sample-2 - Total weight for TTL 169463
Sample-1 - Total weight for TTC 75763.7
Sample-2 - Total weight for TTC 51326.6
Sample-1 - Total weight for TTB 46516.3
Sample-2 - Total weight for TTB 35966.9
*/
double rew_HF_MG5Py8(int hf_class, int truth_corr) {
  if (hf_class==-1)
    {return 75763.7/51326.6;}
  else if (hf_class==0 && truth_corr<=2)
    {return 168079.0/169463;}
  else 
    {return 46516.3/35966.9;}
}
/*
Yields for Nominal (1) and PhHw (2)
Sample-1 - Total weight for TTL 168079
Sample-2 - Total weight for TTL 244758
Sample-1 - Total weight for TTC 75763.7
Sample-2 - Total weight for TTC 41193
Sample-1 - Total weight for TTB 46516.3
Sample-2 - Total weight for TTB 29903.4
*/
double rew_HF_PhHw(int hf_class, int truth_corr) {
  if (hf_class==-1)
    {return 75763.7/41193.0;}
  else if (hf_class==0 && truth_corr<=2)
    {return 168079.0/244758.0;}
  else 
    {return 46516.3/29903.4;}
}

double rew_HF_nominal(int hf_class, int truth_corr) {
  if (hf_class==-1)
    {return 1.30;}
  else if (hf_class==0 && truth_corr<=2)
    {return 0.86;}
  else 
    {return 1.18;}
}

double rew_njet(int njets,int lep=1) {
  if (lep==1){
    if (njets==7) {return 1.0012;}
    else if (njets==8) {return 1.00793;}
    else if (njets==9) {return 1.046;}
    else if (njets==10) {return 1.12039;}
    else if (njets==11) {return 1.16328;}
    else if (njets==12) {return 1.28329;}
    else if (njets==13) {return 1.81445;}
    else if (njets==14) {return 1.96561;}
    else if (njets==15) {return 2.51358;}
    else {return 1.0;}
  }
  else if (lep==2){
    if (njets==5) {return 1.04532;}
    else if (njets==6) {return 1.00292;}
    else if (njets==7) {return 1.01711;}
    else if (njets==8) {return 1.14271;}
    else if (njets==9) {return 1.17148;}
    else if (njets==10) {return 1.24365;}
    else if (njets==11) {return 1.93843;}
    else if (njets==12) {return 1.62397;}
    else if (njets==13) {return 6.07119;}
    else {return 1.0;}
  }
  else return 0.0;
}
