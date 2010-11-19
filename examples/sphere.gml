%
% Test to render a simple unit-sphere.
%

%{ /v /u /face			  % bind arguments
%  0.8 0.2 v point		  % surface color
%  1.0 0.2 1.0			  % kd ks n
%} sphere 1.2 1.0 3.0 translate /s1

{ /v /u /face			  % bind arguments
  0.8 0.2 v point		  % surface color
  1.0 0.2 1.0			  % kd ks n
} sphere /s
				% scene consisting of two spheres
s -1.2 0.0 3.0 translate 	  % sphere at (-1, 0, 3)
s  1.2 1.0 3.0 translate	  % sphere at (1, 1, 3)
union /scene		  	% compose


%s1 s2 union /s

0.5 0.5 0.5 point			% ambient
[ 1 ]					% lights
scene					% object
4					% depth
90.0					% fov
320 240					% wid ht
"sphere.ppm"				% output file
render
