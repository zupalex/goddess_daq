#include "HG_Goddess_Analysis.h"
#include "Sort_TimeDif.h"

using namespace std;

map<int,vector<int>> QQQ5_bad_strips =
{
  {0,{14,15,17,27,28,29,31}},
  {1,{16,25,28,29,30,31}},
  {2,{0,1,2,30,31}},
  {3,{1,26}}
};

void ExvA_SX3_bystrip(string filenames, string treename, string outfile, vector<int> sectors_to_treat = {0,9,10,11}, vector<int> QQQ5_sec = {3},bool gvp_sort = 0)
{
    vector<string> runs = DecodeItemsToTreat ( filenames,"system" );
    
         
     TFile* firstRootFile = new TFile(runs[0].c_str(), "read");
     
     GoddessReacInfos* reacinfo = (GoddessReacInfos*) firstRootFile->FindObjectAny("GoddessReac");
     
     firstRootFile->Close();

    TChain* chain = new TChain ( treename.c_str(),treename.c_str() );
    for ( int h = 0; h<runs.size(); h++ )
    {
        chain->Add ( runs[h].c_str() );
    }
  
    TFile* out_file = new TFile ( outfile.c_str(),"recreate" );
    
    vector<TH2F*> hists;
    string histname; 
    TH2F* hist;

      for (int sectors = 0; sectors<sectors_to_treat.size(); sectors++)
      {
	  for (int strips = 0; strips<4; strips++)
	  {
	    histname = "U"+to_string(sectors_to_treat[sectors])+"s"+to_string(strips);
	    
	    hist = new TH2F(histname.c_str(),"Ex. v. Ang.",500,0,180,400,-10,15); //293 bins for 75 kev width
	    
	    hists.push_back(hist);
	    
	  }

      }
      
            for (int q = 0; q<QQQ5_sec.size(); q++)
      {
	  for (int qq = 0; qq<32; qq++)
	  {
	    histname = "U"+to_string(QQQ5_sec[q])+"s"+to_string(qq);
	    
	    hist = new TH2F(histname.c_str(),"Ex. v. Ang.",500,0,180,400,-10,15); //293 bins for 75 kev width
	    
	    hists.push_back(hist);
	    
	  }

      }
   
      float energy;
      float angle;
      float qval;
      int pos_in_vec;
      
     int entries = chain->GetEntries();
     
     if (gvp_sort == 0){
     
    vector<SiDataBase>* vectdata = new vector<SiDataBase>;
    chain->SetBranchAddress ( "si",&vectdata );
    
    
    for (int entry = 0; entry<entries; entry++)
    {
        chain->GetEntry ( entry );

        if ( entry%200000==0 )
        {
            cout<<"Entries treated: "<<entry<<"/"<<entries<<"\r"<<flush;
        }
        
        for (int event = 0; event<vectdata->size(); event++)
	{
	  SiDataBase* ev = &vectdata->at(event);
	  
	  	    if (ev->isBarrel && ev->isUpstream)
	    {
	  
	  for (int sec = 0; sec<sectors_to_treat.size(); sec++)
	  {
	    if (ev->sector != sectors_to_treat[sec]) continue;
	    
	    energy = ev->ESumLayer();
	    angle = ev->Angle(); 
	    
	    qval = ev->QValue(reacinfo,energy,TMath::DegToRad()*angle);
	    
	    
	    
	    int st = ev->StripMaxLayer();
	    
	    if (st == -1 || st>3) continue;
	    
	    int pos_in_vec = sec*4+st;
/*	    
	    
	    cout<<"Sec: "<<sec<<endl;
	    cout<<"Strip: "<<st<<endl;
	    cout<<pos_in_vec<<endl;*/
	    
	    hists[pos_in_vec]->Fill(angle,reacinfo->qValGsGs-qval);
	    
	  }
	    }
	   else if (ev->isUpstream && !ev->isBarrel)
	    {
	      	  for (int secq = 0; secq<QQQ5_sec.size(); secq++)
	  {
	    if (ev->sector != QQQ5_sec[secq]) continue;
	    
	   energy = ev->ESumLayer();
	   angle = ev->Angle(); 
	    
	   qval = ev->QValue(reacinfo,energy,TMath::DegToRad()*angle);
	    
	    
	    
	    int st = ev->StripMaxLayer();
	    
	    pos_in_vec = sectors_to_treat.size()*4+secq*32+st;
/*	    
	    
	    cout<<"Sec: "<<sec<<endl;
	    cout<<"Strip: "<<st<<endl;
	    cout<<pos_in_vec<<endl;*/
	    
	    hists[pos_in_vec]->Fill(angle,reacinfo->qValGsGs-qval);
	    }
	   
	  
	}
	else{ continue;}
    }
     }
     }
     else {
       
      si_struct* part = new si_struct;
    chain->SetBranchAddress ( "si",&part );

    
    for (int i =0; i<entries; i++)
    {
              chain->GetEntry ( i );

        if ( i%200000==0 )
        {
            cout<<"Entries treated: "<<i<<"/"<<entries<<"\r"<<flush;
        }
       
	  si_struct* ev = part;
	 if (ev->isUpstream && ev->isBarrel && ev->mult == 1)
	 {
	  for (int sec = 0; sec<sectors_to_treat.size(); sec++)
	  {
	    int sector = ev->sector_strip;
	    int strip = round((ev->sector_strip-sector)*100);
	    
	    if (sector == 10 && strip == 1) continue;
	    if (sector == 2 && strip == 2) continue;
	    if (sector == 4 && strip == 2) continue;
	    
	    if (sector != sectors_to_treat[sec]) continue;
	    
	    float energy = ev->energy;
	    float angle = ev->angle; 
	    
	    float qval = SiDataBase::QValue(reacinfo,energy,TMath::DegToRad()*angle);
	    
	    
	    if ( strip>3) continue;
	    
	    int pos_in_vec = sec*4+strip;
/*	    
	    
	    cout<<"Sec: "<<sec<<endl;
	    cout<<"Strip: "<<st<<endl;
	    cout<<pos_in_vec<<endl;*/
	    
	    hists[pos_in_vec]->Fill(angle,reacinfo->qValGsGs-qval);
	    
	  }
	 }
	  
	}
     }
    
   TH2F* tot_hist = new TH2F("tot_hist","Ex. v. Ang.",500,0,180,400,-10,15); //293 bins for 75 kev width
    
    for (int h = 0; h<hists.size(); h++)
    {
      //hists[h]->Draw();
      hists[h]->Write();
      tot_hist->Add(hists[h]);
    }
    
    tot_hist->Write();
    
    out_file->Close();
   
  return;
  
  
  
}


void EPvA_SX3_bystrip(string filenames, string treename, string outfile, vector<int> sectors_to_treat = {0,9,10,11}, bool gvp_sort = 0)
{
    vector<string> runs = DecodeItemsToTreat ( filenames,"system" );

    TChain* chain = new TChain ( treename.c_str(),treename.c_str() );
    for ( int h = 0; h<runs.size(); h++ )
    {
        chain->Add ( runs[h].c_str() );
    }
  
    TFile* out_file = new TFile ( outfile.c_str(),"recreate" );
    
    vector<TH2D*> hists;
    string histname; 
    TH2D* hist;

      for (int sectors = 0; sectors<sectors_to_treat.size(); sectors++)
      {
	  for (int strips = 0; strips<4; strips++)
	  {
	    histname = "U"+to_string(sectors_to_treat[sectors])+"s"+to_string(strips);
	    
	    hist = new TH2D(histname.c_str(),"Ex. v. Ang.",800,0,180,800,0,40);
	    
	    hists.push_back(hist);
	    
	  }

      }
      
     int entries = chain->GetEntries();
     
     if (gvp_sort == 0)
     {
     
    vector<SiDataBase>* vectdata = new vector<SiDataBase>;
    chain->SetBranchAddress ( "si",&vectdata );
    
    
    for (int entry = 0; entry<entries; entry++)
    {
        chain->GetEntry ( entry );

        if ( entry%200000==0 )
        {
            cout<<"Entries treated: "<<entry<<"/"<<entries<<"\r"<<flush;
        }
        
        for (int event = 0; event<vectdata->size(); event++)
	{
	  SiDataBase* ev = &vectdata->at(event);
	  
	  for (int sec = 0; sec<sectors_to_treat.size(); sec++)
	  {
	    
	    if (ev->isUpstream && ev->isBarrel) {
	    
	    if (ev->sector != sectors_to_treat[sec]) continue;
	    
	    double energy = ev->ESumLayer();
	    double angle = ev->Angle();
	    
	    int st = ev->StripMaxLayer();
	    
	    if (st == -1 || st>3) continue;
	    
	    int pos_in_vec = sec*4+st;
	    
	    hists[pos_in_vec]->Fill(angle, energy);
	    }
	    
	    
	    
	  }
	  
	}
    }
     }
     else{
       
      si_struct* part = new si_struct;
    chain->SetBranchAddress ( "si",&part );
    
    for (int i =0; i<entries; i++)
    {
              chain->GetEntry ( i );

        if ( i%200000==0 )
        {
            cout<<"Entries treated: "<<i<<"/"<<entries<<"\r"<<flush;
        }
       
	  si_struct* ev = part;
	if (ev->isUpstream && ev->isBarrel)
	    {  
	  for (int sec = 0; sec<sectors_to_treat.size(); sec++)
	  {
	    int sector = ev->sector_strip;
	    int strip = round((ev->sector_strip-sector)*100);
	    
	     if (sector == 10 && strip == 1) continue;
	    if (sector == 2 && strip == 2) continue;
	    if (sector == 4 && strip == 2) continue;
	    
	    if (sector != sectors_to_treat[sec]) continue;
	    
	    float energy = ev->energy;
	    float angle = ev->angle; 
	    
	    
	    if ( strip>3) continue;
	    
	    int pos_in_vec = sec*4+strip;
/*	    
	    
	    cout<<"Sec: "<<sec<<endl;
	    cout<<"Strip: "<<st<<endl;
	    cout<<pos_in_vec<<endl;*/
	    
	    hists[pos_in_vec]->Fill(angle,energy);
	    }
	    
	  }
	  
	}
     }
    
    TH2F* tot_hist = new TH2F("tot_hist","Ex. v. Ang.",800,0,180,800,0,40);
    for (int h = 0; h<hists.size(); h++)
    {
      hists[h]->Write();
      tot_hist->Add(hists[h]);
    }
    
    tot_hist->Write();
    
    out_file->Close();
   
  return;
  
  
  
}


double rel_q_value(double psi, double T3)
{
	double q_val;
	double M1 = 133.9053947*931.5, M2 = 1876.134, M3 = 938.766;
	double P1, P3;
	double E1, E3;
	double T1 = 764*0.9843957; //after energy loss of beam through half of the target 3769.81

	E1 = T1 + M1;
	E3 = T3 + M3;
	P1 = sqrt((E1 * E1) - (M1 * M1));
	P3 = sqrt((E3 * E3) - (M3 * M3));

	q_val = M1 + M2 - M3 - sqrt((M1 * M1) + (M2 * M2) + (M3 * M3) + 2 * M2 * E1 - 2 * E3 * (E1 + M2) + 2 * P1 * P3 * cos(psi * (3.1415 / 180.0)));

	return q_val;
}

void GamvPartEn(string filenames, string tree, string outfile, vector<int> sx3_sect = {0}, vector<int> qqq5_sect = {}, int gam_i = 0, int gam_f = 110, bool gvp_sort = 0)
{
  
    vector<string> runs = DecodeItemsToTreat ( filenames,"system" );
    
         TFile* firstRootFile = new TFile(runs[0].c_str(), "read");
     
     GoddessReacInfos* reacinfo = (GoddessReacInfos*) firstRootFile->FindObjectAny("GoddessReac");
     
     firstRootFile->Close();

    TChain* chain = new TChain ( tree.c_str(),tree.c_str() );
    for ( int h = 0; h<runs.size(); h++ )
    {
        chain->Add ( runs[h].c_str() );
    }
  
    TFile* out_file = new TFile ( outfile.c_str(),"recreate" );
    
    
         int entries = chain->GetEntries();
	 
	 TH2F* histo = new TH2F("GvP_en", "Gamma energy v. Particle Energy",250,0,15,800,0,4000);
	 
	 float pen;
	 float gen;
     
    if (gvp_sort == 0)
	 {
	 
    vector<SiDataBase>* part = new vector<SiDataBase>;
    chain->SetBranchAddress ( "si",&part );
    
    vector<GamData>* gam = new vector<GamData>;
    chain->SetBranchAddress("gam",&gam);
    
    for (int i =0; i<entries; i++)
    {
              chain->GetEntry ( i );

        if ( i%200000==0 )
        {
            cout<<"Entries treated: "<<i<<"/"<<entries<<"\r"<<flush;
        }
       
        
        for (int event = 0; event<part->size(); event++)
	{
	  SiDataBase* ev = &part->at(event);
	  
	  for (int sec = 0; sec<sx3_sect.size(); sec++)
	  {
	    if (ev->sector == sx3_sect.at(sec) && ev->isBarrel && ev->isUpstream)
	    {
	    float energy = ev->ESumLayer();
	    float angle = ev->Angle(); 
	    
	    //pen = rel_q_value(angle,energy);
	    
	    pen = ev->QValue(reacinfo,energy,TMath::DegToRad()*angle);

	    }
	  }
	  
	}
	
	for (int gevent = 0; gevent<gam->size(); gevent++)
	{
	  GamData* gev = &gam->at(gevent);
	  
	  if (gev->num >= gam_i && gev->num <= gam_f && gev->type == 1)
	  {
	   gen = gev->en/1.233-5.5898; //if Xe please divide by 3
	  }
	  
	}

	
	histo->Fill(6.93-pen,gen);
	
	
	 }
	 
}
else{  

    vector<SiDataBase>* part = new vector<SiDataBase>;
    chain->SetBranchAddress ( "si",&part );
    
    gam_struct* gam = new gam_struct;
    chain->SetBranchAddress("gam",&gam);
    float energy;
    float angle;
    map<int,vector<int>>::iterator itr;
    for (int i =0; i<entries; i++)
    {
              chain->GetEntry ( i );


        if ( i%200000==0 )
        {
            cout<<"Entries treated: "<<i<<"/"<<entries<<"\r"<<flush;
        }
       
       gam_struct* gev = gam;
       gen = gev->gam_energy[1]/1.233-5.5898;  //if Xe divide by 3
       
       if (part->size()<1) continue;
       
        for (int event = 0; event<part->size(); event++)
	{
	  SiDataBase* ev = &part->at(event);
	  
	 for (int sec = 0; sec<sx3_sect.size(); sec++)
	  { 
	    
	    if (ev->sector == sx3_sect[sec] && ev->isBarrel && ev->isUpstream)
	    {
	    if (ev->sector == 10 && ev->StripMaxLayer() == 1) continue;
	    if (ev->sector == 2 && ev->StripMaxLayer() == 2) continue;
	    if (ev->sector == 4 && ev->StripMaxLayer() == 2) continue;

	    float energy = ev->ESumLayer();
	    
// 	    cout<<"E "<<ev->eSum[0]<<endl;
	    
	    float angle = ev->Angle(); 
	    
// 	    cout<<"A "<<angle<<endl;
// 	    pen = rel_q_value(angle,energy);
	    
	    pen = ev->QValue(reacinfo,energy,TMath::DegToRad()*angle);
	    
// 	    cout<<"pen "<<pen<<endl;
	    histo->Fill(5.93-pen,gen);

	    }
	  }
	  
// 	  cout<<"gam "<<gam->gam_energy[1]<<endl;
	  
	    
	  
	  energy = -1;
	  angle = -1;
	    
	    //pen = rel_q_value(angle,energy);	    
	    //pen = SiDataBase::QValue(reacinfo,energy,TMath::DegToRad()*angle);
	    
// 	    && gev->crystal_num[0] >= gam_i && gev->crystal_num[0] <= gam_f
	    
// 	    cout<<gev->crystal_num[0]<<endl;
	    
// 	   if (gev->crystal_num[1]==-1)
// 	  {
// 	     //if Xe divide by 3;
		
// 		cout<<"g "<<gev->gam_energy[0]<<endl;
// 		cout<<"p "<<pen<<endl;
	   
	  }

	    }
// 	 for (int sec2 = 0; sec2<qqq5_sect.size(); sec2++)
// 	  {
// 	    
// 	    if (sector == sx3_sect[sec2] && !ev->isBarrel && ev->isUpstream)
// 	    {
// 	      if (itr == QQQ5_bad_strips.find(sector))
// 	      {
// 		for (int j = 0; j<itr->second.size(); j++)
// 		{
// 		  if (strip == itr->second.at(j)) continue;
// 		}
// 	      }
// // 	      cout<<sector<<endl;
// 	    energy = ev->energy;
// 	    angle = ev->angle; 
// 	    
// 	    if (energy == -1 || angle == -1) continue;
// 	    
// 	    //pen = rel_q_value(angle,energy);	    
// 	    pen = SiDataBase::QValue(reacinfo,energy,TMath::DegToRad()*angle);
// 	    
// 	   if (gev->crystal_num[1]==-1 && gev->crystal_num[0] >= gam_i && gev->crystal_num[0] <= gam_f)
// 	  {
// 	    
// 	   gen = gev->gam_energy[0]/1.233-5.5898;  //if Xe divide by 3;
// 	   
// 	   	histo->Fill(6.93-pen,gen);
// 	  }
// 
// 	    }
// 	  }
	
	
	 }

    
    histo->Write();
    
    out_file->Close();
    
  return;
}


void Geom_4_Harry (string filenames, string outfile, string treename, vector<int> sx3_sect = {0}, vector<int> qqq5_sect = {})
{
      vector<string> runs = DecodeItemsToTreat ( filenames,"system" );

    TChain* chain = new TChain ( treename.c_str(),treename.c_str() );
    for ( int h = 0; h<runs.size(); h++ )
    {
        chain->Add ( runs[h].c_str() );
    }
  
    TFile* out_file = new TFile ( outfile.c_str(),"recreate" );
    
    vector<TH2D*> hists;
    string histname; 
    TH2D* hist;

      for (int sectors = 0; sectors<sx3_sect.size(); sectors++)
      {
	  for (int strips = 0; strips<4; strips++)
	  {
	    histname = "U"+to_string(sx3_sect[sectors])+"s"+to_string(strips);
	    
	    hist = new TH2D(histname.c_str(),"Ex. v. ZPos",800,-180,180,800,0,20);
	    
	    hists.push_back(hist);
	    
	  }

      }
      
     int entries = chain->GetEntries();
     
    vector<SiDataBase>* vectdata = new vector<SiDataBase>;
    chain->SetBranchAddress ( "si",&vectdata );
    
    
    for (int entry = 0; entry<entries; entry++)
    {
        chain->GetEntry ( entry );

        if ( entry%200000==0 )
        {
            cout<<"Entries treated: "<<entry<<"/"<<entries<<"\r"<<flush;
        }
        
        for (int event = 0; event<vectdata->size(); event++)
	{
	  SiDataBase* ev = &vectdata->at(event);
	  
	  for (int sec = 0; sec<sx3_sect.size(); sec++)
	  {
	    if (ev->sector != sx3_sect[sec]) continue;
	    
	    double energy = ev->ESumLayer();
	    TVector3 posvec = ev->PosE1();
	    double pos = posvec[2];
	    if (pos == 0 || pos == 97.8) continue;
	    
	    
	    //cout<<pos<<endl;
	    
	    int st = ev->StripMaxLayer();
	    
	    if (st == -1 || st>3) continue;
	    
	    int pos_in_vec = sec*4+st;
	    
	    hists[pos_in_vec]->Fill(-pos, energy);
	    
	    
	    
	  }
	  
	}
    }
    
    
    TH2D* hist_tot = new TH2D("hist_tot","Ex v. Zpos Total",800,-180,180,800,0,20);
    
    for (int h = 0; h<hists.size(); h++)
    {
      hists[h]->Write();
      
      hist_tot->Add(hists[h]);
    }
    
    hist_tot->Write();
    
    out_file->Close();
  
  
  return;
}


void SolidAngleCalc (float angle_dif, bool isUpstream, bool isBarrel, int sector, TVector3 offset = {0,0,0})
{
  
  float sx3_width = 40.3;
  float barrelrad = 98.43;
  float barrellen = 228.6;
 
  
    long double n_iterations = 1.55E+7;

    double theta_alpha, phi_alpha, rand1, rand2;
    double x0 = 0, y0 = 0, z0 = 0, x1, y1, z1;

    //Setting the geometry of the detector: What alpha angles would be captured by the detector?
    double z_offset;
    double radius = barrelrad; //mm

    double t, x_int, y_int, z_int;
    
    long double b = 0;
    
    float slice_sz;
    float len;
    int slices;
    float beg;
    
    
    if (isBarrel && isUpstream) 
    {
      slices = ceil(35/angle_dif);
      slice_sz = angle_dif/35;
      len = 75*slice_sz;

    }
    else if (!isBarrel && isUpstream)
    {
      slices = ceil(25/angle_dif);
      slice_sz = angle_dif/25;
    }
    else return;
    
    
    
    srand(time(NULL));
  
    for (int slice = 0; slice<slices; slice++)
    {
  //reference detector for SX3 is U0. 
    for(int i=0; i< n_iterations; i++ )
    {
    	rand1 = ((double) rand() / (RAND_MAX));
    	rand2 = ((double) rand() / (RAND_MAX));
	//cout<<rand1<<" "<<rand2<<endl;

    	theta_alpha = acos((2*rand1) - 1.0);
    	phi_alpha = 2*3.14159 * rand2;

    	x1 = sin(theta_alpha) * cos(phi_alpha);
    	y1 = sin(theta_alpha) * sin(phi_alpha);
    	z1 = cos(theta_alpha);

    	t = (radius - y0) / (y1 - y0);

    	x_int = x0 + (t*(x1 - x0));
    	y_int = y0 + (t*(y1 - y0));
    	z_int = z0 + (t*(z1 - z0));
	
	if ( isBarrel && isUpstream) 
	{
	  z_offset = barrellen/2 + offset(2);
	  beg = len*slice + z_offset;
	  z_offset = beg;
	  if (x_int <= 20.0 && x_int >= -20.0 && z_int >= z_offset && z_int <= z_offset + len*(slice + 1) && y1 >= 0) b+=1;
	  //cout<<setprecision(200)<<b/n_iterations<<endl;
	}
	if (!isBarrel && isUpstream)
	{
	  theta_alpha = theta_alpha * 180/3.14;
	  phi_alpha = phi_alpha * 180/3.14;
	  z_offset = barrellen + offset(2);
	  beg = 140+ angle_dif*slice;
	  if (phi_alpha<= 120 && phi_alpha>= 60 && theta_alpha>= beg && theta_alpha >=beg+angle_dif) b+=1;
	}
  
    }
  cout<<"Solid Angle: "<<setprecision(15)<<b/n_iterations<<endl;
  b = 0;
    
    }
 
  return;
}

void SuperimposeTwoFNR (vector<string> f_treat, string out_file)
{
  
  pair<float,float> p;
  
  
  std::vector<std::pair<float, float>> dataPoints;
  
  vector<vector<pair<float,float>>> datasets;
  
  int fval;
  int val;
  

  for (int i = 0; i<f_treat.size(); i++)
  {
    dataPoints.clear();
    
    std::ifstream indata ( f_treat[i].c_str() );

    if ( !indata.is_open() )
    {
        cerr << "Input file does not exist...\n";
        return nullptr;
    }
    
    string readLine;
    
        while ( std::getline ( indata, readLine ) )
    {
        if ( readLine.empty() ) continue;
	
	for (int f = 0; f<readLine.size(); f++)
	{
	  if (isspace(readLine[f])) continue;
	  else 
	  {
	    fval = f;
	    break;
	  }
	}
	
	size_t first_sp = readLine.find(' ',fval);
	
	string first_val = readLine.substr(fval,first_sp-1);
	
	p.first = stof(first_val);
	
	for (int l = first_sp; l < readLine.size(); l++)
	{
	  if (isspace(readLine[l])) continue;
	  else 
	  {val = l;
	    break;
	  }
	}

	  string sec_val = readLine.substr(val,readLine.size());

	  p.second = stof(sec_val);
	  	
	dataPoints.push_back(p);	
    }
    
    datasets.push_back(dataPoints);
    
  }
  
  pair<float,float> np;
  float angle = 0;
  float final_val = 0;
  
  vector<pair<float,float>> newdataset;
  
  for (int ok = 0; ok <datasets[0].size(); ok++)
  {
    np.first = 0;
    np.second = 0;
    
    angle = 0;
    final_val = 0;
    

    for (int fk = 0; fk<datasets.size(); fk++)
    {
    if (datasets[0].size() != datasets[fk].size())
    {
      cout<<"Size of Data Set 0:"<<datasets[0].size()<<endl;
      cout<<"Size of Data Set "<<fk<<":"<<datasets[fk].size()<<endl;
      cout<<"For this to work, all Two FNR data sets need to have the same number of points. Check your iterations and try again."<<endl;
      return;
    }
      
      angle += datasets[fk][ok].first;
      final_val += datasets[fk][ok].second;
    }    
    angle = angle/datasets.size();
        
    np.first = angle;
    np.second = final_val;
        
    newdataset.push_back(np);
    
    
  }
  
  ofstream textfile(out_file.c_str());
  
  for (int ll = 0; ll<newdataset.size(); ll++)
  {
    textfile<<newdataset[ll].first<<" "<<newdataset[ll].second<<endl;
  }
  
  textfile.close();
  
  
  return;
}

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  