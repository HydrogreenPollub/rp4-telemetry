# HMI Parser

## TL;DR
Parser działa w skrócie tak:
 - w swoim programie załączasz hmi_parser.hpp
 - tworzysz instancję klasy HmiParser
 - dla każdego odebranego bajtu wywołujesz processOctet
 - jeżeli processOctet wykryje poprawną ramkę, wywoła odpowiedni callback

## Przykładowy program
Do przykładowego programu zrobiłem możliwość łatwego skompilowania CMakiem, żeby sam pościągał wszystkie zależności. Parser zależy wyłącznie od jednego pliku nagłówkowego crc.hpp z https://github.com/boostorg/crc.git (zależność od spdlog zastąp swoim loggerem). Reszta zależności jest potrzebna jedynie dla programu przykładowego.

Kompilacja jest standardowa:

    mkdir build && cd build
    cmake ..
    cmake --build .

## Callbacki
Przede wszystkim callbacki najprawdopodobniej ulegną jeszcze zmianom, bo dojdą dodatkowe dane itp.

Jako callbacków możesz użyć wszystkiego, co obsługuje std::function.

Dwa pierwsze parametry callbacków to timestamp kiedy dana wiadomość została wysłana przez Mastera, a dokładnie: ile czasu upłynęło od włączenia zasilania Mastera w chwili wysyłania wiadomości:
 - msClockTickCount - liczba milisekund od uruchomienia zasilania
 - cycleClockTickCount - liczba cykli zegara taktowanego 120MHz (32 bity, więc przekręca się co ok. 36 sekund).

Kolejne parametry zależą od konkretnej wiadomości.

### onMasterMeasurements
Ten callback jest wywoływany, gdy przyjdą dane zmierzone bezpośrednio przez Mastera. Tu prawdopodobnie będą największe zmiany. Dane znajdują się w parametrze "measurements".

### onMasterStatus
Ten callback jest wywoływany, gdy przyjdą dane o aktualnym stanie pracy Mastera. Tu prawdopodobnie będą największe zmiany. Dane znajdują się w parametrze "status".

### onProtiumValues
Ten callback będzie wywoływany, gdy przyjdą parametry przesłane ze sterownika Protium do Mastera. Dane znajdują się w parametrze "values".

### onProtiumOperatingState
Ten callback będzie wywoływany, gdy Master wykryje zmianę stanu, w jakim znajduje się Protium. Obecnie wykrywane stany są w enum ProtiumOperatingState. W parametrze "operatingStateLogEntries" masz 8 wpisów z ostatnimi stanami (+timestamp), w jakich znajdował się Protium. Na podstawie timestampów możesz odtworzyć jednolitą historię stanów, w których znajdował się Protium. Chodzi o to, żebyś był w stanie ją odtworzyć i logował, bo to jest bardzo potrzebne do rozwiązywania problemów ze sterownikami Protium, kiedy np. nie chcą się włączyć albo same nie wiadomo czemu się wyłączają.

W przypadku jakiegoś "dziwnego" zachowania sterowników Protium na zawodach, może być bardzo potrzebna możliwość szybkiego przejrzenia zarówno historii ich stanów i wartości parametrów.

Poniżej przesyłam log wygenerowany przez ten przykładowy program, który załączam. 

	[2025-05-08 20:05:01.417] [info] Protium operating state @ 37008ms (146011549 cycles)
	[2025-05-08 20:05:01.418] [info] Current opearting state: CommandNotFound
	[2025-05-08 20:05:01.419] [info] Operating state history:
	[2025-05-08 20:05:01.419] [info]  history[0] @          0ms -> Disconnected
	[2025-05-08 20:05:01.420] [info]  history[1] @          0ms -> Disconnected
	[2025-05-08 20:05:01.421] [info]  history[2] @          0ms -> Disconnected
	[2025-05-08 20:05:01.421] [info]  history[3] @          0ms -> Disconnected
	[2025-05-08 20:05:01.422] [info]  history[4] @          0ms -> Disconnected
	[2025-05-08 20:05:01.422] [info]  history[5] @          0ms -> Disconnected
	[2025-05-08 20:05:01.423] [info]  history[6] @          0ms -> Disconnected
	[2025-05-08 20:05:01.423] [info]  history[7] @      37008ms -> CommandNotFound
	[2025-05-08 20:05:06.424] [info] Protium operating state @ 42008ms (746012798 cycles)
	[2025-05-08 20:05:06.425] [info] Current opearting state: Disconnected
	[2025-05-08 20:05:06.426] [info] Operating state history:
	[2025-05-08 20:05:06.427] [info]  history[0] @          0ms -> Disconnected
	[2025-05-08 20:05:06.427] [info]  history[1] @          0ms -> Disconnected
	[2025-05-08 20:05:06.428] [info]  history[2] @          0ms -> Disconnected
	[2025-05-08 20:05:06.428] [info]  history[3] @          0ms -> Disconnected
	[2025-05-08 20:05:06.429] [info]  history[4] @          0ms -> Disconnected
	[2025-05-08 20:05:06.429] [info]  history[5] @          0ms -> Disconnected
	[2025-05-08 20:05:06.430] [info]  history[6] @      37008ms -> CommandNotFound
	[2025-05-08 20:05:06.430] [info]  history[7] @      42008ms -> Disconnected
	[2025-05-08 20:05:13.415] [info] Protium operating state @ 49014ms (1586792156 cycles)
	[2025-05-08 20:05:13.416] [info] Current opearting state: EnteringToStartingPhase
	[2025-05-08 20:05:13.416] [info] Operating state history:
	[2025-05-08 20:05:13.417] [info]  history[0] @          0ms -> Disconnected
	[2025-05-08 20:05:13.417] [info]  history[1] @          0ms -> Disconnected
	[2025-05-08 20:05:13.417] [info]  history[2] @          0ms -> Disconnected
	[2025-05-08 20:05:13.418] [info]  history[3] @          0ms -> Disconnected
	[2025-05-08 20:05:13.418] [info]  history[4] @          0ms -> Disconnected
	[2025-05-08 20:05:13.418] [info]  history[5] @      37008ms -> CommandNotFound
	[2025-05-08 20:05:13.418] [info]  history[6] @      42008ms -> Disconnected
	[2025-05-08 20:05:13.418] [info]  history[7] @      49014ms -> EnteringToStartingPhase
	[2025-05-08 20:05:13.528] [info] Protium operating state @ 49125ms (1600038692 cycles)
	[2025-05-08 20:05:13.529] [info] Current opearting state: AnodeSupplyPressureCheck
	[2025-05-08 20:05:13.530] [info] Operating state history:
	[2025-05-08 20:05:13.531] [info]  history[0] @          0ms -> Disconnected
	[2025-05-08 20:05:13.531] [info]  history[1] @          0ms -> Disconnected
	[2025-05-08 20:05:13.532] [info]  history[2] @          0ms -> Disconnected
	[2025-05-08 20:05:13.532] [info]  history[3] @          0ms -> Disconnected
	[2025-05-08 20:05:13.533] [info]  history[4] @      37008ms -> CommandNotFound
	[2025-05-08 20:05:13.533] [info]  history[5] @      42008ms -> Disconnected
	[2025-05-08 20:05:13.534] [info]  history[6] @      49014ms -> EnteringToStartingPhase
	[2025-05-08 20:05:13.534] [info]  history[7] @      49125ms -> AnodeSupplyPressureCheck
	[2025-05-08 20:05:13.544] [info] Protium operating state @ 49139ms (1601738873 cycles)
	[2025-05-08 20:05:13.545] [info] Current opearting state: TemperatureCheck
	[2025-05-08 20:05:13.546] [info] Operating state history:
	[2025-05-08 20:05:13.546] [info]  history[0] @          0ms -> Disconnected
	[2025-05-08 20:05:13.547] [info]  history[1] @          0ms -> Disconnected
	[2025-05-08 20:05:13.547] [info]  history[2] @          0ms -> Disconnected
	[2025-05-08 20:05:13.548] [info]  history[3] @      37008ms -> CommandNotFound
	[2025-05-08 20:05:13.549] [info]  history[4] @      42008ms -> Disconnected
	[2025-05-08 20:05:13.549] [info]  history[5] @      49014ms -> EnteringToStartingPhase
	[2025-05-08 20:05:13.550] [info]  history[6] @      49125ms -> AnodeSupplyPressureCheck
	[2025-05-08 20:05:13.550] [info]  history[7] @      49139ms -> TemperatureCheck
	[2025-05-08 20:05:18.552] [info] Protium operating state @ 54139ms (2201726171 cycles)
	[2025-05-08 20:05:18.553] [info] Current opearting state: Disconnected
	[2025-05-08 20:05:18.554] [info] Operating state history:
	[2025-05-08 20:05:18.555] [info]  history[0] @          0ms -> Disconnected
	[2025-05-08 20:05:18.555] [info]  history[1] @          0ms -> Disconnected
	[2025-05-08 20:05:18.556] [info]  history[2] @      37008ms -> CommandNotFound
	[2025-05-08 20:05:18.556] [info]  history[3] @      42008ms -> Disconnected
	[2025-05-08 20:05:18.557] [info]  history[4] @      49014ms -> EnteringToStartingPhase
	[2025-05-08 20:05:18.558] [info]  history[5] @      49125ms -> AnodeSupplyPressureCheck
	[2025-05-08 20:05:18.558] [info]  history[6] @      49139ms -> TemperatureCheck
	[2025-05-08 20:05:18.559] [info]  history[7] @      54139ms -> Disconnected
	[2025-05-08 20:05:22.440] [info] Protium operating state @ 58032ms (2668958790 cycles)
	[2025-05-08 20:05:22.441] [info] Current opearting state: FCGasSystemCheck
	[2025-05-08 20:05:22.441] [info] Operating state history:
	[2025-05-08 20:05:22.442] [info]  history[0] @          0ms -> Disconnected
	[2025-05-08 20:05:22.442] [info]  history[1] @      37008ms -> CommandNotFound
	[2025-05-08 20:05:22.443] [info]  history[2] @      42008ms -> Disconnected
	[2025-05-08 20:05:22.443] [info]  history[3] @      49014ms -> EnteringToStartingPhase
	[2025-05-08 20:05:22.444] [info]  history[4] @      49125ms -> AnodeSupplyPressureCheck
	[2025-05-08 20:05:22.445] [info]  history[5] @      49139ms -> TemperatureCheck
	[2025-05-08 20:05:22.445] [info]  history[6] @      54139ms -> Disconnected
	[2025-05-08 20:05:22.446] [info]  history[7] @      58032ms -> FCGasSystemCheck
	[2025-05-08 20:05:27.447] [info] Protium operating state @ 63032ms (3268892737 cycles)
	[2025-05-08 20:05:27.447] [info] Current opearting state: Disconnected
	[2025-05-08 20:05:27.447] [info] Operating state history:
	[2025-05-08 20:05:27.447] [info]  history[0] @      37008ms -> CommandNotFound
	[2025-05-08 20:05:27.447] [info]  history[1] @      42008ms -> Disconnected
	[2025-05-08 20:05:27.448] [info]  history[2] @      49014ms -> EnteringToStartingPhase
	[2025-05-08 20:05:27.448] [info]  history[3] @      49125ms -> AnodeSupplyPressureCheck
	[2025-05-08 20:05:27.448] [info]  history[4] @      49139ms -> TemperatureCheck
	[2025-05-08 20:05:27.448] [info]  history[5] @      54139ms -> Disconnected
	[2025-05-08 20:05:27.449] [info]  history[6] @      58032ms -> FCGasSystemCheck
	[2025-05-08 20:05:27.449] [info]  history[7] @      63032ms -> Disconnected
	[2025-05-08 20:05:29.415] [info] Protium operating state @ 65007ms (3505901535 cycles)
	[2025-05-08 20:05:29.416] [info] Current opearting state: FCSealingCheck
	[2025-05-08 20:05:29.417] [info] Operating state history:
	[2025-05-08 20:05:29.417] [info]  history[0] @      42008ms -> Disconnected
	[2025-05-08 20:05:29.418] [info]  history[1] @      49014ms -> EnteringToStartingPhase
	[2025-05-08 20:05:29.419] [info]  history[2] @      49125ms -> AnodeSupplyPressureCheck
	[2025-05-08 20:05:29.419] [info]  history[3] @      49139ms -> TemperatureCheck
	[2025-05-08 20:05:29.420] [info]  history[4] @      54139ms -> Disconnected
	[2025-05-08 20:05:29.420] [info]  history[5] @      58032ms -> FCGasSystemCheck
	[2025-05-08 20:05:29.421] [info]  history[6] @      63032ms -> Disconnected
	[2025-05-08 20:05:29.421] [info]  history[7] @      65007ms -> FCSealingCheck
	[2025-05-08 20:05:34.423] [info] Protium operating state @ 70007ms (4105889380 cycles)
	[2025-05-08 20:05:34.423] [info] Current opearting state: Disconnected
	[2025-05-08 20:05:34.424] [info] Operating state history:
	[2025-05-08 20:05:34.424] [info]  history[0] @      49014ms -> EnteringToStartingPhase
	[2025-05-08 20:05:34.424] [info]  history[1] @      49125ms -> AnodeSupplyPressureCheck
	[2025-05-08 20:05:34.424] [info]  history[2] @      49139ms -> TemperatureCheck
	[2025-05-08 20:05:34.425] [info]  history[3] @      54139ms -> Disconnected
	[2025-05-08 20:05:34.425] [info]  history[4] @      58032ms -> FCGasSystemCheck
	[2025-05-08 20:05:34.425] [info]  history[5] @      63032ms -> Disconnected
	[2025-05-08 20:05:34.425] [info]  history[6] @      65007ms -> FCSealingCheck
	[2025-05-08 20:05:34.425] [info]  history[7] @      70007ms -> Disconnected
	[2025-05-08 20:05:38.023] [info] Protium operating state @ 73616ms (244039365 cycles)
	[2025-05-08 20:05:38.024] [info] Current opearting state: FCVoltageCheck
	[2025-05-08 20:05:38.024] [info] Operating state history:
	[2025-05-08 20:05:38.025] [info]  history[0] @      49125ms -> AnodeSupplyPressureCheck
	[2025-05-08 20:05:38.025] [info]  history[1] @      49139ms -> TemperatureCheck
	[2025-05-08 20:05:38.026] [info]  history[2] @      54139ms -> Disconnected
	[2025-05-08 20:05:38.026] [info]  history[3] @      58032ms -> FCGasSystemCheck
	[2025-05-08 20:05:38.027] [info]  history[4] @      63032ms -> Disconnected
	[2025-05-08 20:05:38.027] [info]  history[5] @      65007ms -> FCSealingCheck
	[2025-05-08 20:05:38.028] [info]  history[6] @      70007ms -> Disconnected
	[2025-05-08 20:05:38.028] [info]  history[7] @      73616ms -> FCVoltageCheck
	[2025-05-08 20:05:38.038] [info] Protium operating state @ 73622ms (244780098 cycles)
	[2025-05-08 20:05:38.039] [info] Current opearting state: EnteringToRunningPhase
	[2025-05-08 20:05:38.040] [info] Operating state history:
	[2025-05-08 20:05:38.040] [info]  history[0] @      49139ms -> TemperatureCheck
	[2025-05-08 20:05:38.040] [info]  history[1] @      54139ms -> Disconnected
	[2025-05-08 20:05:38.041] [info]  history[2] @      58032ms -> FCGasSystemCheck
	[2025-05-08 20:05:38.041] [info]  history[3] @      63032ms -> Disconnected
	[2025-05-08 20:05:38.041] [info]  history[4] @      65007ms -> FCSealingCheck
	[2025-05-08 20:05:38.042] [info]  history[5] @      70007ms -> Disconnected
	[2025-05-08 20:05:38.042] [info]  history[6] @      73616ms -> FCVoltageCheck
	[2025-05-08 20:05:38.042] [info]  history[7] @      73622ms -> EnteringToRunningPhase
	[2025-05-08 20:05:39.078] [info] Protium values @ 74675ms (371135013 cycles)
	[2025-05-08 20:05:39.079] [info] FC_V = 46.6400, FC_A = 0.8000, FC_W = 37.2000, Energy = 0.0000, FCT1 = 28.8100, FAN = 9.0000
	[2025-05-08 20:05:39.080] [info] H2P1 = 0.4700, H2P2 = 0.4700, TankP = 143.2000, TankT = 0.0000, V_Set = 47.0000, I_Set = 3.0000
	[2025-05-08 20:05:39.080] [info] UCB_V = 40.4600, Stasis_selector = 1.0000, STASIS_V1 = 46.2000, STASIS_V2 = 46.8000, Number_of_cell = 60
	[2025-05-08 20:05:39.080] [info] Protium operating state @ 74675ms (371113008 cycles)
	[2025-05-08 20:05:39.081] [info] Current opearting state: Running
	[2025-05-08 20:05:39.081] [info] Operating state history:
	[2025-05-08 20:05:39.081] [info]  history[0] @      54139ms -> Disconnected
	[2025-05-08 20:05:39.081] [info]  history[1] @      58032ms -> FCGasSystemCheck
	[2025-05-08 20:05:39.082] [info]  history[2] @      63032ms -> Disconnected
	[2025-05-08 20:05:39.082] [info]  history[3] @      65007ms -> FCSealingCheck
	[2025-05-08 20:05:39.082] [info]  history[4] @      70007ms -> Disconnected
	[2025-05-08 20:05:39.082] [info]  history[5] @      73616ms -> FCVoltageCheck
	[2025-05-08 20:05:39.082] [info]  history[6] @      73622ms -> EnteringToRunningPhase
	[2025-05-08 20:05:39.083] [info]  history[7] @      74675ms -> Running