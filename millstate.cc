#include "millstate.hh"

MillState::MillState()
{
	for (int i = 0; i < 24; ++i) cs[i] = -1;
	notplaced[0] = notplaced[1] = 9;
	onboard[0]   = onboard[1]   = 0;
	removed[0]    = removed[1]    = 0;
}

void MillState::add(int player, int pos)
{
	Q_ASSERT(notplaced[player] > 0 && cs[pos] == -1);
	notplaced[player]--;
	onboard[player]++;
	cs[pos] = player;
}

void MillState::move(int from, int to)
{
	Q_ASSERT(cs[from] != -1 && cs[to] == -1);
	cs[to] = cs[from];
	cs[from] = -1;
}

void MillState::remove(int pos)
{
	Q_ASSERT(cs[pos] != -1);
	int player = cs[pos];
	onboard[player]--;
	removed[player]++;
	cs[pos] = -1;
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
	eas.reserve(9);
	for (int i = 0; i < 24; ++i) {
		if (cs[i] == player && !ismill(i)) eas.append(i);
	}
	return eas;
}

QList<MillState> MillState::possibilities(int player) const
{
	QList<MillState> poss;
	poss.reserve(32);

	if (notplaced[player] > 0) {
		for (int i = 0; i < 24; ++i) {
			if (cs[i] == -1) {
				MillState clone = *this;
				clone.add(player, i);
				eataftermill(poss, clone, player, i);
			}
		}
	} else if (onboard[player] <= 3) {
		QVector<int> occupied;
		occupied.reserve(3);
		for (int i = 0; i < 24; ++i) {
			if (cs[i] == player) {
				occupied << i;
			}
		}
		for (int j = 0; j < 24; ++j) {
			if (cs[j] == -1) {
				for (int i_ = 0; i_ < occupied.size(); ++i_) {
					int i = occupied[i_];
					MillState clone = *this;
					clone.move(i, j);
					eataftermill(poss, clone, player, j);
				}
			}
		}
	} else {
		for (int i = 0; i < 24; ++i) {
			if (cs[i] == player) {
				for (int j_ = 0; j_ < connectedto[i].size(); ++j_) {
					int j = connectedto[i][j_];
					if (cs[j] == -1) {
						MillState clone = *this;
						clone.move(i, j);
						eataftermill(poss, clone, player, j);
					}
				}
			}
		}
	}

	return poss;
}

int MillState::countMoves(int player) const
{
	int count;
	for (int i = 0; i < 24; ++i) {
		if (cs[i] == player) {
			for (int j_ = 0; j_ < connectedto[i].size(); ++j_) {
				int j = connectedto[i][j_];
				if (cs[j] == -1) {
					count++;
				}
			}
		}
	}
	return count;
}

bool MillState::operator ==(const MillState& other) const
{
	for (int k : {0, 1}) {
		if (notplaced[k] != other.notplaced[k]) return false;
		if (onboard[k] != other.onboard[k]) return false;
		if (removed[k] != other.removed[k]) return false;
	}
	for (int k = 0; k < 24; ++k) {
		if (cs[k] != other.cs[k]) return false;
	}
	return true;
}

bool MillState::operator <(const MillState& other) const
{
	for (int i = 0; i < 24; ++i) {
		if (cs[i] < other.cs[i]) return true;
		if (cs[i] > other.cs[i]) return false;
	}
	for (int k : {0, 1}) {
		if (notplaced[k] < other.notplaced[k]) return true;
		if (notplaced[k] > other.notplaced[k]) return false;
		if (onboard[k] < other.onboard[k]) return true;
		if (onboard[k] > other.onboard[k]) return false;
		if (removed[k] < other.removed[k]) return true;
		if (removed[k] > other.removed[k]) return false;
	}
	return false;
}

void MillState::eataftermill(QList<MillState>& poss, MillState& state, int player, int pos) const
{
	if (state.ismill(pos)) {
		QVector<int> eas = state.eatable(1 - player);
		if (eas.isEmpty()) {
			poss << state;
		} else {
			for (int a_; a_ < eas.size(); ++a_) {
				int a = eas[a_];
				MillState clone = state;
				clone.remove(a);
				poss.prepend(clone); // try to beter order to improve alphabeta cutoff
			}
		}
	} else {
		poss << state;
	}
}

uint qHash(const MillState& key, uint seed)
{
	uint r = seed;
	for (int i = 0; i < 24; ++i) {
		r ^= (key.cs[i] << i);
	}
	r ^= (key.notplaced[0] << 25);
	r ^= (key.notplaced[1] << 28);
	return r;
}
