from ROOT import TFile, TH1F, TCanvas, TLegend
from collections import OrderedDict as OD
from cluster import Cluster
from hist_auxiliary import createhist, PlotStyle, fillWithOverFlow 
from plot_auxiliary import formatLegend, createLatex
from tree_auxiliary import get_branch_array 
import sys
#:/import tracemalloc

def load_tree(tree, branches, cluster_type, r_dict={},
             h_size_tot_ac=0, h_charge_tot_ac=0, h_barycenter_tot_ac=0):

    _dict = OD()
    for idx, entry in enumerate(range(tree.GetEntries())):
        if (entry % 1000000) == 0: 
            print(f"Scanning {cluster_type} clusters: ", entry,"/",tree.GetEntries())
        #print(tracemalloc.get_traced_memory())
        tree.GetEntry(entry)
        event = branches['event'][0]
        if r_dict:
            if event not in r_dict.keys():
                continue
        if 'dead' in cluster_type and branches['size'][0] == 0:
            continue
        detId = branches['detId'][0]
        if event not in _dict.keys():
            _dict[event] = OD()
        if detId not in _dict[event].keys():
            _dict[event][detId] = OD()
        if 'dead' in cluster_type:
           _dict[event][detId][entry] = Cluster(entry, event, branches['run'][0], branches['lumi'][0], branches['detId'][0], -1,
                                                -1, -1, branches['size'][0], -1)
        else:
            _dict[event][detId][entry] = Cluster(entry, event, branches['run'][0], branches['lumi'][0], branches['detId'][0], branches['firstStrip'][0],
                                             branches['endStrip'][0], branches['barycenter'][0], branches['size'][0], branches['charge'][0])
        if h_size_tot_ac:
           h_size_tot_ac.Fill( branches['size'][0] ); 
           h_charge_tot_ac.Fill( branches['charge'][0] );
           h_barycenter_tot_ac.Fill( branches['barycenter'][0] )
    return _dict

def LHCC_rawprime_clusters():
     
    faster = 1;
    expTag = "clusterStudy";
    output = TFile("cluster_study_test.root", "recreate")

    f1 = TFile("RawPrimeFlatTupleInt8.root", "read");
    _1 = f1.Get("dump_rawprime");
    onlineClusterTree = _1.Get("onlineClusterTree");

    f2 = TFile("Muon_sep19_2_2_dump_raw.root", "read");
    _2 = f2.Get("sep19_2_2_dump_raw");
    offlineClusterTree = _2.Get("offlineClusterTree");

    f3 = TFile("RawPrimeFlatTupleInt8.root", "read");
    _3 = f3.Get("dead_dump_rawprime");
    onlineDeadStripTree = _3.Get("deadStripTree");

    f4 = TFile("Muon_sep19_2_2_dead_raw.root", "read");
    _4 = f4.Get("sep19_3_dump_deadStrips");
    offlineDeadStripTree = _4.Get("deadStripTree");

    nMax = 5000;

    #print('Loaded files; ', tracemalloc.get_traced_memory())

    h_size_tot_ac =createhist("RAWp_online_size", "(online) raw' cluster; size; yield",50, 0., 50. );
    h_charge_tot_ac = createhist("RAWp_online_charge", "(online) raw' cluster; charge; yield",88, 0., 704. );
    h_barycenter_tot_ac = createhist("RAWp_online_barycenter", "(online) raw' cluster; barycenter; yield",950, 0., 950. );
 
    #print('created hist: ', tracemalloc.get_traced_memory())


    branches_r = {}
    branches_rp = {}
    branches_dead_r = {}
    branches_dead_rp = {}

    branch_names = OD([('event', 1),
                       ('run', 1),
                       ('lumi', 1),
                       ('detId', 1),
                       ('firstStrip', 1),
                       ('endStrip', 1),
                       ('barycenter', 1),
                       ('size', 1),
                       ('charge', 1),
                       ('channel', nMax),
                       ('adc', nMax)
        ])

    for branch_name, size in branch_names.items():
        branches_r[branch_name] = get_branch_array(offlineClusterTree, branch_name, size)
        #print(tracemalloc.get_traced_memory())
        branches_rp[branch_name] = get_branch_array(onlineClusterTree, branch_name, size)
        #print(tracemalloc.get_traced_memory())
        if size == nMax:
            size = 800
        if branch_name in ['firstStrip', 'endStrip', 'barycenter', 'charge', 'adc']:
            continue
        branches_dead_r[branch_name] = get_branch_array(offlineDeadStripTree, branch_name, size)
        #print(tracemalloc.get_traced_memory())
        branches_dead_rp[branch_name] = get_branch_array(onlineDeadStripTree, branch_name, size)
        #print(tracemalloc.get_traced_memory())
    
    r_dict = load_tree(offlineClusterTree, branches_r, 'raw')
    #print(tracemalloc.get_traced_memory())
    rp_dict = load_tree(onlineClusterTree, branches_rp, 'rawp', r_dict,
                           h_size_tot_ac, h_charge_tot_ac, h_barycenter_tot_ac)
    r_d_dict = load_tree(offlineDeadStripTree, branches_dead_r, 'dead strip', r_dict)
    rp_d_dict = load_tree(onlineDeadStripTree, branches_dead_rp, 'dead_p strip', rp_dict)

    h_size_tot_sc = createhist("RAW_offline_size", "(offline) raw cluster; size; yield", 50, 0., 50. );
    h_charge_tot_sc = createhist("RAW_offline_charge", "(offline) raw cluster; charge; yield", 88, 0., 704. );
    h_barycenter_tot_sc =createhist("RAW_offline_barrycenter", "(offline) raw cluster barycenter; yield",950, 0., 950. );

    for evt in r_dict.keys():
        if evt not in rp_dict.keys():
            print("[Error] Unexpected, found unmatched event in raw!\n")
            del r_dic[evt]
            continue
                        
        for detid in r_dict[evt]: 
            for sc_idx in r_dict[evt][detid]:
                _sc = r_dict[evt][detid][sc_idx]
                h_size_tot_sc.Fill( _sc.size );
                h_charge_tot_sc.Fill( _sc.charge );
                h_barycenter_tot_sc.Fill( _sc.barycenter);

    #gStyle.SetOptTitle(0);
    #gErrorIgnoreLevel = kWarning;
    canvSingle0 = TCanvas("canvSingle0", "canvSingle0", 700, 600)
    canvSingle0.GetPad(0).SetMargin (0.18, 0.20, 0.12, 0.07);
    PlotStyle(h_size_tot_ac)
    h_size_tot_ac.SetLineColor(46)
    PlotStyle(h_size_tot_sc); 
    h_size_tot_sc.SetLineWidth(1); 
    h_size_tot_sc.SetFillColorAlpha(31, 0.4);
    h_size_tot_sc.SetLineColorAlpha(31, 0.4)

    h_size_tot_ac.GetYaxis().SetRangeUser(
                h_size_tot_ac.GetYaxis().GetXmin(),
                h_size_tot_ac.GetMaximum()*1.2 );
    h_size_tot_ac.Draw("HIST");
    h_size_tot_sc.Draw("HIST same");
    
    leg0 = TLegend(.62, .6, .87, .8);
    leg0.AddEntry(h_size_tot_sc, "RAW", "f");
    leg0.AddEntry(h_size_tot_ac, "RAW\'", "l");
    formatLegend(leg0);
    leg0.Draw();
    
    latex = createLatex()
    latex.DrawLatexNDC(0.33,0.945,"2023 pp Data #sqrt{s_{NN}} = 5.36 TeV");
    
    canvSingle0.SaveAs("../img_8bit/"+expTag+"_TotalClusters_size.png");


    PlotStyle(h_charge_tot_ac)
    h_charge_tot_ac.SetLineColor(46)
    PlotStyle(h_charge_tot_sc); 
    h_charge_tot_sc.SetLineWidth(1); 
    h_charge_tot_sc.SetFillColorAlpha(31, 0.4);
    h_charge_tot_sc.SetLineColorAlpha(31, 0.4)

    h_charge_tot_ac.GetYaxis().SetRangeUser(
                h_charge_tot_ac.GetYaxis().GetXmin(),
                h_charge_tot_ac.GetMaximum()*1.2 );
    h_charge_tot_ac.Draw("HIST");
    h_charge_tot_sc.Draw("HIST same");
    leg0.Draw();
    latex.DrawLatexNDC(0.33,0.945,"2023 pp Data #sqrt{s_{NN}} = 5.36 TeV");
    canvSingle0.SaveAs("../img_8bit/"+expTag+"_TotalClusters_charge.png");


    PlotStyle(h_barycenter_tot_ac)
    h_barycenter_tot_ac.SetLineColor(46)
    PlotStyle(h_barycenter_tot_sc);
    h_barycenter_tot_sc.SetLineWidth(1); 
    h_barycenter_tot_sc.SetFillColorAlpha(31, 0.4);
    h_barycenter_tot_sc.SetLineColorAlpha(31, 0.4)

    h_barycenter_tot_ac.Draw("hist");
    h_barycenter_tot_sc.Draw("hist same");
    h_barycenter_tot_ac.Draw("hist same")
    latex.DrawLatexNDC(0.33,0.945,"2023 pp Data #sqrt{s_{NN}} = 5.36 TeV");
    canvSingle0.SaveAs("../img_8bit/"+expTag+"_TotalClusters_barycenter.png")

    
    output.cd()
    h_size_tot_sc.Write()
    h_size_tot_ac.Write()
    h_barycenter_tot_ac.Write()
    h_barycenter_tot_sc.Write()
    output.Close()

    del h_size_tot_sc
    del h_charge_tot_sc
    del h_barycenter_tot_sc
    del h_size_tot_ac
    del h_charge_tot_ac
    del h_barycenter_tot_ac

    matched_sc2ac = OD()
    unmatched_scs = []
    unmatched_acs = []

    for evt_p in rp_dict.keys():
        for detid_p in rp_dict[evt_p]:
            for ac_idx_p in rp_dict[evt_p][detid_p]:
                ac = rp_dict[evt_p][detid_p][ac_idx_p]
                distance = 9999.
                closest_sc = Cluster()
                closest_sc.idx = -999
                for sc_idx in r_dict[evt_p][detid_p]:
                    sc = r_dict[evt_p][detid_p][sc_idx]
                    if ( (ac.size == sc.size) and (max(ac.firstStrip, sc.firstStrip ) <= min( ac.endStrip,   sc.endStrip )) ):
                            if (abs(ac.barycenter - sc.barycenter) < distance):
                                 closest_sc = sc
                                 distance  = abs(ac.barycenter - sc.barycenter);
                if closest_sc.idx != -999:
                    if ( closest_sc.idx not in matched_sc2ac.keys() ):
                        if (ac.firstStrip == closest_sc.firstStrip):
                            closest_sc.matchLevel = 1;
                            ac.matchLevel = 1;
                        else:                            
                            closest_sc.matchLevel = 2;
                            ac.matchLevel = 2;
                        matched_sc2ac[ closest_sc.idx ] = ac.idx;
                    else:
                        prev_ac = rp_dict[evt_p][detid_p][matched_sc2ac[closest_sc.idx]]
                        if (abs( ac.barycenter - closest_sc.barycenter ) < \
                            abs( prev_ac.barycenter - closest_sc.barycenter ) ):
                               unmatched_acs.append(prev_ac);
                               matched_sc2ac[ closest_sc.idx ] = ac.idx;
                else:
                   unmatched_acs.append(ac)

    for evt in  r_dict.keys():
      for detid in r_dict[evt]:
        for sc_idx in r_dict[evt][detid]:
            sc = r_dict[evt][detid][sc_idx]
            if sc.idx not in matched_sc2ac.keys():
                unmatched_scs.append(sc)

    print(f"[Summary] matched_sc2ac.size(): {len(matched_sc2ac)}, unmatched_acs.size(): {len(unmatched_acs)}, unmatched_scs.size(): {len(unmatched_scs)}")

    h_charge  = createhist( "charge", "; RAW SiStripCluster charge; RAW' ApproxCluster charge", 88, 0., 704., 88, 0., 704., one_d=False );
    h_barycenter = createhist( "barycenter", "; RAW SiStripCluster barycenter; RAW' ApproxCluster barycenter", 192, 0., 768., 192, 0., 768., False );
    h_barycenter_vs_charge = createhist( "barycenter_vs_charge","; #Delta barycenter (RAW'-RAW); #Delta charge (RAW'-RAW)", 100, -2., 2., 100, -700, 300, False );

    h_charge_res = createhist( "chagre_res","; charge (RAW'-RAW)/RAW; yield",\
                               50, -.1, .1);
    h_barycenter_res = createhist( "barycenter_res","; barycenter RAW'-RAW; yield",\
                                    50, -.1, .1)

    with open('../img_8bit/matched_scs.txt', 'w') as f:
      for idx_sc, idx_ac in matched_sc2ac.items():
        offlineClusterTree.GetEntry(idx_sc);
        onlineClusterTree.GetEntry(idx_ac);
        assert(branches_r['size'][0] == branches_rp['size'][0])
        f.write(f'{branches_r["event"][0]} {branches_r["detId"][0]} {idx_sc} {branches_r["barycenter"][0]} {branches_r["size"][0]} {branches_r["charge"][0]} {branches_r["firstStrip"][0]} {branches_r["endStrip"][0]} {idx_ac} {branches_rp["barycenter"][0]} {branches_rp["size"][0]} {branches_rp["charge"][0]} {branches_rp["firstStrip"][0]} {branches_rp["endStrip"][0]}"\n" ')
        h_charge.Fill( branches_r['charge'][0], branches_rp['charge'][0] );
        h_barycenter.Fill( branches_r['barycenter'][0], branches_rp['barycenter'][0] );
        h_barycenter_vs_charge.Fill(branches_rp['barycenter'][0] - branches_r['barycenter'][0],branches_rp['charge'][0] - branches_r['charge'][0] );

        charge_withovrflow = fillWithOverFlow(h_charge_res, ( branches_rp['charge'][0] - branches_r['charge'][0] )/(branches_r['charge'][0]), 1 );
        fillWithOverFlow(h_barycenter_res, branches_rp['barycenter'][0] - branches_r['barycenter'][0], 1)

    PlotStyle(h_charge_res)
    h_charge_res.Draw("")
    latex.DrawLatexNDC(0.60,0.80,"Mean={mean:.2f}".format(mean=h_charge_res.GetMean()));
    latex.DrawLatexNDC(0.60,0.75,"Std Dev={std:.2f}".format(std=h_charge_res.GetStdDev()));
    canvSingle0.SetLogy(True);
    canvSingle0.SaveAs("../img_8bit/"+expTag+"_MatchedClusters_charge_res.png")
    del h_charge_res

    canvSingle0.SetLogy(0)
    h_barycenter.GetZaxis().SetTitleOffset(1.8);
    h_barycenter.GetZaxis().SetTitle("number of clusters");
    h_barycenter.Draw("COLZ")
    PlotStyle(h_barycenter)
    h_barycenter.Draw("hist")
    latex.DrawLatexNDC(0.33,0.945,"2023 PbPb Data #sqrt{s_{NN}} = 5.36 TeV");
    canvSingle0.SaveAs("../img_8bit/"+expTag+"_MatchedClusters_barycenter_scat.png")
    del h_barycenter

    PlotStyle(h_barycenter_vs_charge)
    h_barycenter_vs_charge.Draw("COLZ")
    canvSingle0.SaveAs("../img_8bit/"+expTag+"_MatchedClusters_del_barycenter_del_charge_scat.png")
    del h_barycenter_vs_charge

    del canvSingle0

    with open('../img_8bit/unmatched_scs.txt', 'w') as f:
      f.write('event detId sc_idx barycenter size charge firstStrip endStrip\n')
      for sc in unmatched_scs:
          f.write(f'{sc.event} {sc.detId} {sc.idx} {sc.barycenter} \
                 {sc.size} {sc.charge} {sc.firstStrip} {sc.endStrip} "\n"')

    with open('../img_8bit/unmatched_acs.txt', 'w') as f:
       f.write('event detId ac_idx barycenter size charge firstStrip endStrip\n')
       for ac in unmatched_acs:
          f.write(f'{ac.event} {ac.detId} {ac.idx} {ac.barycenter} {ac.size} {ac.charge} \
                  {ac.firstStrip} {ac.endStrip}"\n"')

    canv2 = TCanvas("canv2", "canv2", 600, 600);

    for sc in unmatched_scs:
       offlineClusterTree.GetEntry(sc.idx);
       if not faster: sc.print()
                
       if( sc.event not in rp_dict.keys() ): continue;
       if( sc.detId not in rp_dict[ sc.event ].keys() ):
            print(f"[Warning:1] no matched ac detId {sc.detId} in event {sc.event} for sc\n")
            print("sc: ")
            sc.print();
            print("offline dead strip (sc) ")
            for r_ds in r_d_dict[ sc.event ][ sc.detId ]: r_ds.print();
            print("online dead strip (sc) ")
            for rp_ds in rp_d_dict[ sc.event ][ sc.detId ]: rp_ds.print();
            continue;
       #canv2 = TCanvas("canv2", "canv2", 600, 600);
       h_sc = createhist( f'sc{sc.idx}', f"RAW ({sc.firstStrip}-{sc.endStrip}); strip; ADC", 800,0,800 );

       for i in range(branches_r['size'][0]): h_sc.Fill(branches_r['channel'][i], branches_r['adc'][i]);
       h_sc.SetLineWidth(0);
       h_sc.SetFillColorAlpha(31, 0.4);
       h_sc.SetLineColorAlpha(31, 0.4);
       h_sc.GetYaxis().SetRangeUser(0, 
       h_sc.GetMaximum()*3);
       h_sc.DrawClone("hist");

       ac_map = rp_dict[sc.event][sc.detId];
       if not faster: print(f"found {len(ac_map)} ac in same event and same detId\n")

       foundOverlap = False
       for ac_idx, ac in ac_map.items():
            onlineClusterTree.GetEntry(ac_idx);
            if not faster: ac.print();

            if not foundOverlap:
               foundOverlap =  max( ac.firstStrip, sc.firstStrip ) <= \
                               min( ac.endStrip,   sc.endStrip );

            h_ac = createhist(f"ac_{sc.idx}_{ac.idx}", f"RAW' ({branches_rp['firstStrip'][0]}-{branches_rp['endStrip'][0]}); strip; ADC", 800,0,800 );
            for i in range(branches_rp['size'][0]): h_ac.Fill(branches_rp['channel'][i], branches_rp['adc'][i]);
            h_ac.SetLineColor(46);
            #h_ac.DrawClone("hist same")
            del h_ac

       prefix = ""
       if not foundOverlap:
           print(f"[Warning:2] no matched ac in event {sc.event} detId {sc.detId} for sc\n")
           prefix = "Warning2_";
           print("sc: ")
           sc.print();
           print("ac: ")
           for ac in ac_map.values():
               print('approx:')
               ac.print();
       r_ds_map = {}
       if sc.detId in r_d_dict[sc.event].keys(): r_ds_map = r_d_dict[sc.event][sc.detId];
       if not faster: print(f"found len(r_ds_map) online (raw') ds in same event and same detId\n")

       if (len(r_ds_map)==1):
          for r_ds_idx, r_ds in r_ds_map.items():
              offlineDeadStripTree.GetEntry(r_ds_idx);

          h_r_ds = createhist(f"r_ds_{sc.idx}_{r_ds.idx}", f"RAW dead ({r_ds.firstStrip}-{r_ds.endStrip}); strip; ADC", 800,0,800 );
          r_d_channel_min = 1000;
          r_d_channel_max = -1;
          for i in range(branches_dead_r['size'][0]):
              h_r_ds.Fill(branches_dead_r['channel'][i], h_sc.GetMaximum());
              if (branches_dead_r['channel'][i] < r_d_channel_min): r_d_channel_min = branches_dead_r['channel'][i];
              if (branches_dead_r['channel'][i] > r_d_channel_max): r_d_channel_max = branches_dead_r['channel'][i];

          h_r_ds.SetTitle(f"RAW dead ({r_d_channel_min}-{r_d_channel_max});");
          h_r_ds.SetFillColorAlpha(1, 0.2);
          h_r_ds.SetLineColorAlpha(1, 0.2);
          #h_r_ds.DrawClone("hist same");
          del h_r_ds;
       elif len(r_ds_map) == 0:
          pass
       else:
          print("[Error] Don't expect\n");
          sys.exit();

       rp_ds_map = {}
       if sc.detId in rp_d_dict[sc.event].keys(): rp_ds_map = rp_d_dict[sc.event][sc.detId];
       print(rp_ds_map)
       if (len(rp_ds_map)==1):
          for rp_ds_idx, rp_ds in rp_ds_map.items():
              onlineDeadStripTree.GetEntry(rp_ds_idx);
               
          h_rp_ds = createhist(f"rp_ds_{sc_idx}_{rp_ds_idx}", f"RAW' dead {rp_ds_idx}; strip; ADC", 800,0,800 );
          rp_d_channel_min = 1000;
          rp_d_channel_max = -1;
          for i in range(branches_dead_rp['size'][0]):
              h_rp_ds.Fill(branches_dead_rp['channel'][i], h_sc.GetMaximum());
              if (branches_dead_rp['channel'][i] < rp_d_channel_min): rp_d_channel_min = branches_dead_rp['channel'][i];
              if (branches_dead_rp['channel'][i] > rp_d_channel_max): rp_d_channel_max = branches_dead_rp['channel'][i];
                        
          h_rp_ds.SetTitle(f"RAW' dead ({rp_d_channel_min}-{rp_d_channel_max});");
          h_rp_ds.SetFillColorAlpha(3, 0.3);
          h_rp_ds.SetLineColorAlpha(3, 0.3);
          #h_rp_ds.DrawClone("hist same");
          del h_rp_ds;
       elif len(rp_ds_map)==0:
           pass
       else:
            print(f"[Error] Don't expect\n");
            sys.exit(0);
       leg = canv2.BuildLegend(.5, .6, .85, .8);
       canv2.GetPad(0).Modified();
       canv2.GetPad(0).Update();
       del h_sc;
                
       canv2.SaveAs(f"../img_8bit/{expTag}_{prefix}UnmatchedStripClusters_idx{sc.idx}.png");

       canv2.Clear()
       if ( not faster or prefix=="Warning2_"): print("===========================================\n");
    del canv2

#tracemalloc.start() 
LHCC_rawprime_clusters()

