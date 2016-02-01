#include "millstate.hh"

MillState::MillState()
{
	for (int i = 0; i < 24; ++i) cs[i] = -1;
	notplaced[0] = notplaced[1] = 9;
	onboard[0]   = onboard[1]   = 0;
	killed[0]    = killed[1]    = 0;
}

void MillState::add(int player, int pos)
{
	//if (notplaced[player] > 0 && cs[pos] == -1) {
		notplaced[player]--;
		onboard[player]++;
		cs[pos] = player;
	//}
}

void MillState::move(int from, int to)
{
	//if (cs[from] != -1 && cs[to] == -1) {
		cs[to] = cs[from];
		cs[from] = -1;
	//}
}

void MillState::remove(int pos)
{
	//if (cs[pos] != -1) {
		int player = cs[pos];
		onboard[player]--;
		killed[player]++;
		cs[pos] = -1;
	//}
}

bool MillState::ismill(int pos) const
{
	for (int i : {0, 1}) {
		const int *m = millsfrom[pos][i];
		if (cs[m[0]] == cs[m[1]] && cs[m[0]] == cs[m[2]]) return true;
	}
	return false;
}

QVector<int> MillState::eatable(int player) const
{
	QVector<int> eas;
	for (int i = 0; i < 24; ++i) {
		if (cs[i] == player && !ismill(i)) eas.append(i);
	}
	return eas;
}

QVector<MillState> MillState::possibilities(int player) const
{
	QVector<MillState> ps;

	if (notplaced[player] > 0) {
		for (int i = 0; i < 24; ++i) {
			if (cs[i] == -1) {
				MillState clone = *this;
				clone.add(player, i);
				eataftermill(ps, clone, player, i);
			}
		}
	} else if (onboard[player] <= 3) {
		QVector<int> occupied;
		for (int i = 0; i < 24; ++i) {
			if (cs[i] == player) {
				occupied << i;
			}
		}
		for (int j = 0; j < 24; ++j) {
			if (cs[j] == -1) {
				for (int i : occupied) {
					MillState clone = *this;
					clone.move(i, j);
					eataftermill(ps, clone, player, j);
				}
			}
		}
	} else {
		for (int i = 0; i < 24; ++i) {
			if (cs[i] == player) {
				for (int j : connectedto[i]) {
					if (cs[j] == -1) {
						MillState clone = *this;
						clone.move(i, j);
						eataftermill(ps, clone, player, j);
					}
				}
			}
		}
	}

	return ps;
}

void MillState::eataftermill(QVector<MillState>& poss, MillState& state, int player, int pos) const
{
	if (state.ismill(pos)) {
		QVector<int> eas = state.eatable(1 - player);
		if (eas.isEmpty()) {
			poss << state;
		} else {
			for (int a : eas) {
				MillState clone = state;
				clone.remove(a);
				poss << clone;
			}
		}
	} else {
		poss << state;
	}
}
