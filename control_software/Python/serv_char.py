# Perry Petiet
# 23-10-2024
# EasyDsp BLE services and characteristics.

# Indexes services. Selects channel and EQ for interface.
service_indexes = "00000001-0000-1000-8000-00805f9b34fb"
# Indexes characteristics.
char_chan_index = "00000002-0000-1000-8000-00805f9b34fb"
char_is_output  = "00000003-0000-1000-8000-00805f9b34fb"
char_eq_index   = "00000004-0000-1000-8000-00805f9b34fb"

# Equalizer service. Represents equalizer of set index.
service_equalizer = "00000005-0000-1000-8000-00805f9b34fb"
# Equalizer characteristics.
char_q            = "00000006-0000-1000-8000-00805f9b34fb"
char_s            = "00000007-0000-1000-8000-00805f9b34fb"
char_bandwith     = "00000008-0000-1000-8000-00805f9b34fb"
char_boost        = "00000009-0000-1000-8000-00805f9b34fb"
char_freq         = "0000000a-0000-1000-8000-00805f9b34fb"
char_gain         = "0000000b-0000-1000-8000-00805f9b34fb"
char_filt_type    = "0000000c-0000-1000-8000-00805f9b34fb"
char_phase        = "0000000d-0000-1000-8000-00805f9b34fb"
char_state        = "0000000e-0000-1000-8000-00805f9b34fb"

# Mux service. Represents mux for selected output (Index service represents an output)
service_mux  = "0000000f-0000-1000-8000-00805f9b34fb"
#mux service characteristics.
char_mux_val = "00000010-0000-1000-8000-00805f9b34fb"
