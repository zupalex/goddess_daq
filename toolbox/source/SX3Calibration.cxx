#include "SX3Calibration.h"


using namespace std;

float SX3Calibration::GetSlope(pair< float, float >xs, pair< float, float > ys)
{
  float slope = 1;
  
  slope = (ys.second-ys.first)/(xs.first-xs.second);
  
  return slope;
}



vector<pair<float,float>> SX3Calibration::ParallelGains(TH1D* histo)
{
 
 int nbins = histo->GetXaxis()->GetNbins();
 
 int nslices = nbins/2;
 
 float binwidth = histo->GetBinWidth(0);
  
 
 vector<float> integral;
 vector<int> binset;
 
  
  for (int i = 5; i<nbins-5; i++)
  {
    
    int first = i;
    int last = i + 1;
    float hint = histo->Integral(first,last);
    
    //cout<<first<<" "<<hint<<endl;
    

    integral.push_back(hint);
    binset.push_back(first);
    
    
  }
  
  vector<int>jump_start_stop;
  jump_start_stop.push_back(5);
  vector<bool> overlaptype;
  overlaptype.push_back(true);
  
  for (int j = 0; j<integral.size(); j++)
  {
    if (integral[j]< 0.8*integral[j-1] || integral[j]>1.15*integral[j-1])
    {
      if (binset[j]-jump_start_stop.back()>6)
      {
	jump_start_stop.push_back(binset[j]);
	
	if (integral[j]>1.15*integral[j-1]) overlaptype.push_back(true);
	else overlaptype.push_back(false);
      
//       cout<<"Before J: "<<integral[j-1]<<endl;
//       cout<<"J: "<<integral[j]<<endl;
//       cout<<"Bin: "<<binset[j]<<endl;
      }
      
    }
  }
  
  int number_jumps = jump_start_stop.size()/2 +6;
  vector<pair<float,float>> pairs;
  pair<float,float> pair;
  float start_j;
  float end_j;
  
  for (int k = 0; k<number_jumps; k++)
  {
    if (overlaptype[k])
    {
     start_j = histo->GetBinCenter(jump_start_stop[k]);
    }
    else if (!overlaptype[k])
    {
      start_j = histo->GetBinCenter(jump_start_stop[k]+3);
    }
    
    
//     if (jump_start_stop[k+1] == jump_start_stop.back()) 
//      {
//       end_j = histo->GetBinCenter(jump_start_stop[k+1]+10);
// 	
//      }
/*   else*/ if (overlaptype[k+1])
    {
      end_j = histo->GetBinCenter(jump_start_stop[k+1]);
    }
    else if(!overlaptype[k+1])
    {
      
      end_j = histo->GetBinCenter(jump_start_stop[k+1]+3);
    }
    
    float dif = end_j-start_j; 
    
    cout<<dif<<" "<<start_j<<" "<<end_j<<endl;
    
    if (dif <200 || dif > 1000000 ) continue;
    
    pair.first = start_j;
    
    pair.second = end_j;
    
    //cout<<start_j<<" "<<end_j<<endl;
    
    
    pairs.push_back(pair);
 
    
  }
  
return pairs;
}

float area(float x1, float y1, float x2, float y2, float x3, float y3) 
{ 
   return abs((x1*(y2-y3) - x2*(y1-y3)+ x3*(y1-y2))/2.0); 
}

void SX3Calibration::ParallelGains(vector<TH1D*> histos, float pedx, float pedy,int sector, int strip, bool isupstream)
{
  vector<pair<float,float>> pairsy = ParallelGains(histos[0]);
  vector<pair<float,float>> pairsx = ParallelGains(histos[1]);

    
    if (pairsx.size()>pairsy.size() || pairsy.size()>pairsx.size())
    {
      cout<<"One vector is larger than the other. This is not correct. Check the function ParallelGains before continuing."<<endl;
      
      return;
      
    }
    
    vector<float> slopes;
    float slope;
    vector<pair<float,float>> coordinates;
    pair<float,float> set;
       
    pairsy.back().second = pairsy.back().second+75;
    pairsx.back().second = pairsx.back().second+75;
    
    for (int i = 1; i<pairsx.size(); i++)
    {
      reverse(pairsy.begin(),pairsy.end());

      
      slope = GetSlope(pairsx[i],pairsy[i-1]);
      
//       cout<<"Pairsx: "<<pairsx[i].first<<" "<<pairsx[i].second<<endl;
//       cout<<"Pairsy: "<<pairsy[i].first<<" "<<pairsy[i].second<<endl;
//       cout<<"Slope: "<<slope<<endl;
      
      
      set.first = pairsx[i].first;
      set.second = pairsy[i-1].second;
      coordinates.push_back(set);
      set.first = pairsx[i].second;
      set.second = pairsy[i-1].first;
      coordinates.push_back(set);
      
      slopes.push_back(slope);
    }
    
    vector<float> areas;
    
    for (int a = 0; a<slopes.size(); a++)
    {
      areas.push_back(area(pedx,pedy,coordinates[a*2].first,coordinates[a*2].second,coordinates[a*2+1].first,coordinates[a*2+1].second));
      
      cout<<pedx<<" "<<pedy<<" First coor: "<<coordinates[a*2].first<<" "<<coordinates[a*2].second<<" Second coor: "<<coordinates[a*2+1].first<<" "<<coordinates[a*2+1].second<<endl;
    }
    
    ofstream filename;
    
    filename.open("slopes_coordinates_areas.txt");
    
    filename<<"sector,strip,isupstream,number of segments, pedestal y, pedestal x"<<endl;
    filename<<"segment,coordinates left, coordinates right, slope, area"<<endl;
    filename<<sector<<","<<strip<<","<<isupstream<<","<<slopes.size()<<","<<pedy<<","<<pedx<<","<<endl;
    
    for (int seg = 0; seg<slopes.size(); seg++)
    {
      filename<<seg<<",("<<coordinates[seg*2].first<<","<<coordinates[seg*2].second<<"),("<<coordinates[seg*2+1].first<<","<<coordinates[seg*2+1].second<<"),"<<slopes[seg]<<","<<areas[seg]<<","<<endl;
    }
    
    
    
    
  
  return;
}

void SX3Calibration::GainCorrection(string filename, string outfile, string treename, int sector, int strip, bool isupstream, string slopefile)
{
  TFile* file = new TFile(filename.c_str(),"read");
  
  TTree* tree = ( TTree* ) file->Get ( treename.c_str() );

    if ( tree == NULL )
    {

        tree = ( TTree* ) file->Get ( ( ( std::string ) "trees/"+filename ).c_str() );

        if ( tree == NULL )
        {
            return;
        }
    }
  
    TFile* out_file = new TFile ( outfile.c_str(),"update" );
    
    TH2D* nearvfar = new TH2D("nearvfar","Near vs. Far",800,0,2000,800,0,2000);
    
    ifstream txtfile;
    
    txtfile.open(slopefile.c_str());
    
    if (!txtfile)
    {
      cout<<"Unable to open file. Exiting now."<<endl;
      exit(1);
    }
    
    string line;
    stringstream ssline;
    
    params p;
    seg_params sp;
    
    vector<params> par;
    vector<seg_params> spar;
    
    string substr;
    
    vector<float> paramvec;
    float value;
    
    int linetype;
    
    while (getline(txtfile,line))
    {
      linetype = 2;
      
      ssline.str(line);
      cout<<ssline.str().length()<<endl;
      while (ssline.good())
      {
	cout<<"inwhile"<<endl;
	getline(ssline,substr,',');
	cout<<substr<<endl;
	if (substr == "sector" || substr == "segment") 
	{linetype = 0;
	  break;
	}
	
	if (ssline.str().length()<30)
	{ 
	  linetype = 1;
	}
	
	if (substr.at(0) == '(') substr.erase(0,1);
	if (substr.back() == ')') substr.erase(substr.size()-1,1);
      
      	value = stof(substr);
	  
	paramvec.push_back(value);

      }
      cout<<linetype<<endl;
      cout<<"outwhile"<<endl;
      
      if (linetype == 0) break;
      if (linetype == 1)
      {
	p.sector = paramvec[0];
	p.strip = paramvec[1];
	p.isupstream = paramvec[2];
	p.num_seg = paramvec[3];
	p.pedestal_y = paramvec[4];
	p.pedestal_x = paramvec[5];
	
	par.push_back(p);
	
	paramvec.clear();
      }
      cout<<"after first if"<<endl;
      
      if (linetype == 2)
      {
	sp.segment = paramvec[0];
	sp.coorl.first = paramvec[1];
	sp.coorl.second = paramvec[2];
	sp.coorr.first = paramvec[3];
	sp.coorr.second = paramvec[4];
	sp.slope = paramvec[5];
	sp.area = paramvec[6];
	
	spar.push_back(sp);
	
	paramvec.clear();
	
      }

      cout<<"after second if"<<endl;
      
    }

    int entries = tree->GetEntries();
    
    vector<SiDataDetailed>* SiData = new vector<SiDataDetailed>();
    tree->SetBranchAddress ( "si", &SiData );
    
    for (int entry = 0; entry < entries; entry++)
    {
      tree->GetEntry(entry);
      
        if ( entry%20000 == 0 )
        {
            cout << "Treated entry sorted # " << entry << " / " << entries << "\r" << flush;
        }

        if ( SiData->size() ==0 )
        {
            continue;
        }

        for (int y = 0; y<SiData->size(); y++)
	{
      
	  SiDataDetailed* e = &SiData->at(y);
	  
	  
	 
	  
	  
	}
    }
  
  return;
}



void SX3Calibration::PositionCalibration(string filenames, string tree, string outfile, int sector, int strip, vector<float> left_to_right_section, vector<float> y_sections, float alpha = 5.813)
{
  /*left to right section means the left x coordinate of the section then the y coordinate of the section. Do this until each section is listed. 
   Please list sections from left to right as well. */
  /*If sections do not need to be moved in the y direction -- i.e. no real energy leakage to backsides -- please enter the value of the alpha line*/
  
  int num_sec = left_to_right_section.size()/2;
  
  if (left_to_right_section.size()%2!=0) 
  {
    cout<<"Cannot have an odd number of coordinates for sections. Please check again and rerun function."<<endl;
    return;
  }
  else 
  {
    cout<<"Number of sections is: "<< num_sec <<endl;
  }
  
  vector<float> overlap_centers;
  vector<bool> overlap;
  float center = -3;
  
  for (int n = 0; n < num_sec-1; n++)
  {
    if (left_to_right_section.at(n*2+1) > left_to_right_section.at(n*2+2))
    {
     center =  (left_to_right_section.at(n*2+1) - left_to_right_section.at((n+1)*2))/2;
     overlap_centers.push_back(center);
     overlap.push_back(1);
    }
    else overlap.push_back(0);
    center = -3;
  }
  
  overlap.push_back(0);
  
 vector<float> y_corr;
 float corr;
 
 for (int y=0; y<y_sections.size(); y++)
 {
   if (y_sections.at(y) == alpha)
   {
     y_corr.push_back(1);
     continue;
   }
   else{
   corr = alpha/y_sections.at(y);
   
   y_corr.push_back(corr);
   }
   
 }
  
  overlap.push_back(0);
  
  vector<string> runs = DecodeItemsToTreat ( filenames,"system" );
    
         TFile* firstRootFile = new TFile(runs[0].c_str(), "read");
     
     GoddessReacInfos* reacinfo = (GoddessReacInfos*) firstRootFile->FindObjectAny("GoddessReac");
     
     firstRootFile->Close();

     
    TChain* chain = new TChain ( tree.c_str(),tree.c_str() );
    for ( int h = 0; h<runs.size(); h++ )
    {
        chain->Add ( runs[h].c_str() );
    }
  
    TFile* out_file = new TFile ( outfile.c_str(),"update" );
    
    string hist_name = "sector_"+to_string(sector)+"_strip_"+to_string(strip);
    
    TH2D* hist = new TH2D(hist_name.c_str(), "Corrected Pos Cal SX3", 500,-2,2,500,0,10);
    
    int TID = 2200+sector;
    
    int entries = chain->GetEntries();
     

    vector<SiDataDetailed>* part = new vector<SiDataDetailed>;
    chain->SetBranchAddress ( "si",&part );

    
    for (int i =0; i<entries; i++)
    {
              chain->GetEntry ( i );

        if ( i%200000==0 )
        {
            cout<<"Entries treated: "<<i<<"/"<<entries<<"\r"<<flush;
        }
        
       for (int event = 0; event < part->size(); event++)
       {
        
        SiDataDetailed* e = &part->at(event);
	
	for (int f = 0; f<e->E1.en.p.size(); f++)
	{
	
	if (e->isUpstream && e->isBarrel && e->telescopeID==TID)
	{
	  if (e->E1.strip.p.at(f)==strip)
	  {
	{
	float o_pos = (e->E1.en.p.at(f)-e->E1.en.n.at(f))/e->ESumLayer();
	float o_en = e->ESumLayer();
	
	for (int w_sec = 0; w_sec<num_sec; w_sec++)
	{
	  if (o_pos > left_to_right_section[w_sec*2] && o_pos < left_to_right_section[w_sec*2+1])
	    
	  {

	    if (w_sec%2 ==0)
	    {

	      if (overlap[w_sec])
	      {
		if (o_pos>left_to_right_section[w_sec*2+2]) continue;
	      }
	      if (w_sec-1 >0 && overlap[w_sec-1])
	      {
		if (o_pos<left_to_right_section[w_sec*2-1])continue;
	      }
	    
		if (y_corr.at(w_sec) != 1) o_en = o_en*y_corr.at(w_sec);
		
		hist->Fill(o_pos,o_en);
	    }
	    else if (w_sec %2 !=0)
	    {
	      if (overlap[w_sec-1])
	      {
		if (o_pos<left_to_right_section[w_sec*2-1]) continue;
	      }
	      if (w_sec<overlap.size() && overlap[w_sec])
	      {
		if (o_pos>left_to_right_section[w_sec*2+2]) continue;
	      }
	      
	      if (y_corr.at(w_sec) != 1) o_en = o_en*y_corr.at(w_sec);
	      
		hist->Fill(o_pos,o_en);
	    }
	  //float o_percent = (abs(
	}
	}
	
	}
	  }
	}
	}
	
	
       }
        
    }
    
    ofstream filename;
    
    string name = "Position_Corrections_"+to_string(sector)+"_"+to_string(strip)+".txt";
    
    filename.open(name.c_str());
    
    filename<<"sector,strip,isupstream,number of segments"<<endl;
    filename<<"segment,left_end, right_end, y_correction"<<endl;
    filename<<sector<<","<<strip<<", isUpstream"<<","<<num_sec<<","<<endl;
    for (int n = 0; n<num_sec; n++)
    {
      filename<<n<<","<<left_to_right_section[n*2]<<","<<left_to_right_section[n*2+1]<<","<<y_corr[n]<<","<<endl;
    }
    filename.close();
    
    hist->Write();
    out_file->Close();
  
  
return;
}


void SX3Calibration::PosCalCheck(string filenames, string treename, string outfile, vector<int> sectors_to_treat = {0,9,10,11})
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
	    
	    hist = new TH2F(histname.c_str(),"PosCal Check",400,-40,40,400,0,10); 
	    
	    hists.push_back(hist);
	    
	  }

      }
      
    int entries = chain->GetEntries();
    
    float energy;
    float pos;
     
    vector<SiDataBase>* vectdata = new vector<SiDataBase>;
    chain->SetBranchAddress ( "si",&vectdata );
    
    
    for (int entry = 0; entry<entries; entry++)
    {
        chain->GetEntry ( entry );

        if ( entry%20000==0 )
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
// 	    cout<<ev->PosCh_SX3.size()<<endl;
	    pos = ev->PosCh_SX3.at(0);
	    
	    
	    
	    int st = ev->StripMaxLayer();
	    
	    if (st == -1 || st>3) continue;
	    
	    int pos_in_vec = sec*4+st;
/*	    
	    
	    cout<<"Sec: "<<sec<<endl;
	    cout<<"Strip: "<<st<<endl;
	    cout<<pos_in_vec<<endl;*/
	    
	    hists[pos_in_vec]->Fill(pos,energy);
	    
	  }
	    }
	  
	  
	}
	
    }
    
  for (int h = 0; h<hists.size(); h++)
    {
      //hists[h]->Draw();
      hists[h]->Write();
    }
    out_file->Close();
	
  
  return;
}


double quadratic_fit(double* x, double* par)
{
  return par[0]+par[1]*x[0]+par[2]*pow(x[0],2);
}

void SX3Calibration::FitSmiles (TH2F* histo, int sector, int strip, bool isUpstream, float x_0, float x_1)
{
  SX3Calibration sx3;
  
  TF1* fit = new TF1("fit func", quadratic_fit,x_0,x_1, 3);
  
   fit->SetParameters(0.,1.);
   fit->SetLineColor(kRed);
   histo->Fit(fit);
   histo->Draw("colz");
   fit->Draw("same");
   
    ofstream filename;
    
    string name = "Braces_Corrections_"+to_string(sector)+"_"+to_string(strip)+"_"+to_string(x_0)+".txt";
    
    filename.open(name.c_str());
    
    filename<<"sector,strip,isupstream,x_0,x_1,y-intercept, par 1, par 2"<<endl;
    filename<<sector<<","<<strip<<","<< isUpstream <<","<<x_0<<","<<x_1<<","<<fit->GetParameter(0)<<","<<fit->GetParameter(1)<<","<<fit->GetParameter(2)<<endl;
    filename.close();
  
    
  return;
}

void SX3Calibration::Braces(string filenames, string treename, int sector, int strip, bool isUpstream, float y_int, float par1, float par2)
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
 
    TH2F* hist = new TH2F("histo","Braces -- Smile Correction", 400,50,50,400,4,8);
      
    int entries = chain->GetEntries();
    
    float energy;
    float pos;
     
    vector<SiDataDetailed>* vectdata = new vector<SiDataDetailed>;
    chain->SetBranchAddress ( "si",&vectdata );
    
    
    for (int entry = 0; entry<entries; entry++)
    {
        chain->GetEntry ( entry );

        if ( entry%20000==0 )
        {
            cout<<"Entries treated: "<<entry<<"/"<<entries<<"\r"<<flush;
        }
        
        for (int event = 0; event<vectdata->size(); event++)
	{
	  SiDataDetailed* ev = &vectdata->at(event);
	  
	   if (ev->isBarrel && ev->isUpstream && ev->sector == sector && ev->StripMaxLayer() == strip)
	    {
	    
	    energy = ev->ESumLayer();
	    pos = ev->PosCh_SX3.at(0);
	    energy = (energy/(y_int + pos*par1 + pow(pos,2)*par2))*energy;
	    
	    hist->Fill(pos,energy);
	  
	  
	}
	
    }
    }
  
  hist->GetYaxis()->SetRangeUser(4,10);
  hist->GetXaxis()->SetRangeUser(-50,50);
  
  hist->Draw("colz");
  
  return;
}

