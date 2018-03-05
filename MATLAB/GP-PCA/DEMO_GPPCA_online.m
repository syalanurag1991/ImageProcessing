clear all
close all

sigma=20;
randn('seed', 2);
%ima = double(imread('data/house.png'));
ima = double(imread('house.png'));
ima_nse = ima + sigma * randn(size(ima));

figure('Position',[100 0  800 800]);
plotimage(ima_nse);
title(sprintf('Noisy: \n  PSNR %.2f', psnr(ima, ima_nse)));
set(get(gca,'Title'),'FontSize',12);
%% LOCAL VERSION: PLPCA.m
[WP factor_thr hW] = PLPCA_best_params(sigma);
threshold = factor_thr * sigma;
disp(threshold);
% Shift/Redudancy  parameter for the searching zone.
delta = hW-4; %< 2*hW+2P;
func_thresholding = @(ima_ppca) ...
    hardthresholding(ima_ppca, threshold, sigma);
local_time=tic;
%% Patchization step 
ima_patchs = spatial_patchization(ima_nse, WP+2);
%% Local PCA denoising step
ima_patchs_fil = PLPCA_denoising(ima_patchs, func_thresholding, hW, delta);
%% Uniform reprojection step
ima_fil_PLPCA = reprojection_UWA(ima_patchs_fil);
local_time=toc(local_time);
%% Display
figure('Position',[200 0  800 800])
plotimage(ima_fil_PLPCA);
title(sprintf('PLPCA: \n  PSNR %.2f', psnr(ima, ima_fil_PLPCA)));
set(get(gca,'Title'),'FontSize',12);
sprintf('PLPCA:\n PSNR: %.2f dB \n Computing Time: %.2f s.', psnr(ima, ima_fil_PLPCA),local_time)
